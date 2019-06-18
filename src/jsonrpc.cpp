#include "polkadot.h"

CJsonRpc::CJsonRpc(IWebSocketClient *wsc, ILogger *logger, JsonRpcParams params) {
    this->jsonrpc = params.jsonrpc;
    this->wsc = wsc;
    this->logger = logger;
    this->lastId = 1;
}

int CJsonRpc::getNextId() { return ++lastId; }

Json CJsonRpc::request(Json jsonMap) {

    int currentId = this->getNextId();

    // build request
    Json request = Json::object{
        {"id", currentId}, {"jsonrpc", this->jsonrpc}, {"method", jsonMap["method"]}, {"params", jsonMap["params"]},
    };

    // subscribe for response
    this->wsc->registerMessageObserver(this);
    int err = wsc->connect();
    usleep(2000000);
    if ((err == 0) && (wsc->isConnected())) {
        wsc->send(request.dump());
        usleep(2000000);

        string message = "Message " + std::to_string(currentId) + " was sent";
        logger->info(message);
        wsc->disconnect();
    } else {
        string errstr = std::to_string(err);
        logger->error(errstr);
        throw "Connection error: " + errstr;
    }

    usleep(500000);
    Json result = queries.find(currentId)->second;
    queries.erase(currentId);

    return result;
}

void CJsonRpc::handleMessage(const string &payload) {
    Json json;
    string err;
    this->logger->info("Message received: " + payload);
    json = Json::parse(payload, err);
    this->queries.insert(pair<int, Json>(json["id"].int_value(), json["result"]));
}