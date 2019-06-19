#include "polkadot.h"

CJsonRpc::CJsonRpc(IWebSocketClient *wsc, ILogger *logger, JsonRpcParams params)
    : _jsonrpcVersion(params.jsonrpcVersion), _lastId(0), _logger(logger), _wsc(wsc) {
    // subscribe for responses
    _wsc->registerMessageObserver(this);
}

int CJsonRpc::getNextId() { return ++_lastId; }

int CJsonRpc::connect() {
    int err = _wsc->connect();
    if ((err == 0) && (_wsc->isConnected())) {
        return PAPI_OK;
    }
    return PAPI_NO_CONNECTION;
}

void CJsonRpc::disconnect() { _wsc->disconnect(); }

Json CJsonRpc::request(Json jsonMap) {

    // Generate new request Id and place request in query map
    _queryMtx.lock();
    condition_variable completionCV; // Condition variable used to notify about response
    mutex completionMtx;             // Mutex for condition varaiable
    JsonRpcQuery query;
    query.id = getNextId();
    query.completionCV = &completionCV;
    query.completionMtx = &completionMtx;
    _queries[query.id] = query;
    _queryMtx.unlock();

    // build request
    Json request = Json::object{
        {"id", query.id},
        {"jsonrpc", _jsonrpcVersion},
        {"method", jsonMap["method"]},
        {"params", jsonMap["params"]},
    };

    // Send the command
    if (_wsc->isConnected()) {
        _wsc->send(request.dump());

        string message = "Message " + std::to_string(query.id) + " was sent";
        _logger->info(message);
    } else {
        string errstr("Not connected");
        _logger->error(errstr);
        throw JsonRpcException(errstr);
    }

    // Block until a timeout happens or response is received
    std::unique_lock<std::mutex> responseWaitLock(*query.completionMtx);
    query.completionCV->wait_for(responseWaitLock, std::chrono::seconds(1));

    // Move response object and return it
    Json result = move(_queries[query.id].json);
    _queries.erase(query.id);

    return move(result);
}

void CJsonRpc::handleMessage(const string &payload) {

    string err;
    _logger->info("Message received: " + payload);
    Json json = Json::parse(payload, err);

    int requestId = json["id"].int_value();

    _queryMtx.lock();
    if (_queries.count(requestId)) {
        // Response for requestId arrived. Set response and notify
        _queries[requestId].json = json["result"];
        _queries[requestId].completionCV->notify_all();
    }
    _queryMtx.unlock();
}

int CJsonRpc::subscribeWs(Json jsonMap, IWebSocketMessageObserver *observer) {
    // Send normal request
    auto response = request(jsonMap);

    // Get response for this request and extract subscription ID
    int subscriptionId = response["result"].int_value();
    _wsSubscribers[subscriptionId] = observer;

    return 0;
}

int CJsonRpc::unsubscribeWs(Json jsonMap, IWebSocketMessageObserver *observer) {
    // Send normal request
    request(jsonMap);

    // Find observer in the oberver map and remove
    int subscriptionId = 0;
    for (auto it = _wsSubscribers.begin(); it != _wsSubscribers.end(); ++it)
        if (it->second == observer)
            subscriptionId = it->first;
    if (subscriptionId)
        _wsSubscribers.erase(subscriptionId);

    return 0;
}
