#include "polkadot.h"

CJsonRpc::CJsonRpc(IWebSocketClient *wsc, ILogger *logger, JsonRpcParams params)
    : _jsonrpcVersion(params.jsonrpcVersion), _lastId(0), _logger(logger), _wsc(wsc) {
    // subscribe for responses
    _wsc->registerMessageObserver(this);
}

int CJsonRpc::getNextId() { return ++_lastId; }

int CJsonRpc::connect(string node_url) {
    int err = _wsc->connect(node_url);
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
        {"id", query.id}, {"jsonrpc", _jsonrpcVersion}, {"method", jsonMap["method"]}, {"params", jsonMap["params"]},
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
    query.completionCV->wait_for(responseWaitLock, std::chrono::seconds(RESPONSE_TIMEOUT_S));

    // Move response object and return it
    Json result = move(_queries[query.id].json);
    _queries.erase(query.id);

    return move(result);
}

void CJsonRpc::handleMessage(const string &payload) {
    string err;
    _logger->info(string("Message received: ") + payload);
    Json json = Json::parse(payload, err);

    int requestId = 0;
    int subscriptionId = 0;
    if (!json["id"].is_null())
        requestId = json["id"].int_value();
    if (!json["params"].is_null())
        subscriptionId = json["params"]["subscription"].int_value();

    // no react with health response
    if (requestId == INT_MAX)
        return;

    if (requestId && _queries.count(requestId)) {
        // Response for requestId arrived. Set response and notify
        _queryMtx.lock();
        _queries[requestId].json = json["result"];
        condition_variable *completionCV = _queries[requestId].completionCV;
        _queryMtx.unlock();
        completionCV->notify_all();
    } else if (subscriptionId) {
        // Subscription response arrived.
        // Delay it if there are pending subscriptions to prevent update arriving before subscriber is listening
        // TODO: DOT-55, fix with proper producer-consumer
        _queryMtx.lock();
        bool observerFound = (_wsSubscribers.count(subscriptionId) != 0);
        _queryMtx.unlock();
        if (!observerFound) {
            usleep(500000);
        }

        _queryMtx.lock();
        if (_wsSubscribers.count(subscriptionId))
            _wsSubscribers[subscriptionId]->handleWsMessage(subscriptionId, json["params"]["result"]);
        _queryMtx.unlock();
    } else {
        _logger->error("Unknown type of response: " + payload);
    }
}

int CJsonRpc::subscribeWs(Json jsonMap, IWebSocketMessageObserver *observer) {
    // Send normal request
    auto response = request(jsonMap);
    // Get response for this request and extract subscription ID
    int subscriptionId = response.int_value();
    _wsSubscribers[subscriptionId] = observer;
    _logger->info(string("Subscribed with subscription ID: ") + to_string(subscriptionId));

    return subscriptionId;
}

int CJsonRpc::unsubscribeWs(int subscriptionId) {
    // Send unsubscribe request
    if (subscriptionId) {
        Json unsubscribeQuery =
            Json::object{{"method", "state_unsubscribeStorage"}, {"params", Json::array{subscriptionId}}};

        request(unsubscribeQuery);
        _logger->info(string("Unsubscribed from subscription ID: ") + to_string(subscriptionId));

        // remove observer from the oberver map
        _queryMtx.lock();
        _wsSubscribers.erase(subscriptionId);
        _queryMtx.unlock();
    }
    return 0;
}
