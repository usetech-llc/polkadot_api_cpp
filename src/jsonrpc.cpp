#include "polkadot.h"

CJsonRpc::CJsonRpc(IWebSocketClient *wsc, ILogger *logger, JsonRpcParams params) {
    this->jsonrpc = params.jsonrpc;
    _wsc = wsc;
    this->logger = logger;
    this->lastId = 0;

    // subscribe for responses
    _wsc->registerMessageObserver(this);
}

int CJsonRpc::getNextId() { return ++lastId; }

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
    condition_variable cv; // Condition variable used to notify about response
    mutex m;               // Mutex for condition varaiable
    JsonRpcQuery query{.id = getNextId(), .cv = &cv, .m = &m};
    _queries[query.id] = query;
    _queryMtx.unlock();

    // build request
    Json request = Json::object{
        {"id", query.id},
        {"jsonrpc", this->jsonrpc},
        {"method", jsonMap["method"]},
        {"params", jsonMap["params"]},
    };

    // Send the command
    if (_wsc->isConnected()) {
        _wsc->send(request.dump());

        string message = "Message " + std::to_string(query.id) + " was sent";
        logger->info(message);
    } else {
        string errstr("Not connected");
        logger->error(errstr);
        throw JsonRpcException(errstr);
    }

    // Block until a timeout happens or response is received
    std::unique_lock<std::mutex> lk(*query.m);
    query.cv->wait(lk);

    // Move response object and return it
    Json result = move(_queries[query.id].json);
    _queries.erase(query.id);

    return move(result);
}

void CJsonRpc::handleMessage(const string &payload) {

    string err;
    logger->info("Message received: " + payload);
    Json json = Json::parse(payload, err);

    int requestId = json["id"].int_value();

    _queryMtx.lock();
    if (_queries.count(requestId)) {
        // Response for requestId arrived. Set response and notify
        _queries[requestId].json = json["result"];
        _queries[requestId].cv->notify_all();
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
    _wsSubscribers.erase(subscriptionId);

    return 0;
}
