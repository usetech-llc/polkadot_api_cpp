struct JsonRpcParams {
    string jsonrpc;
};

class CJsonRpc : public IMessageObserver {
private:
    std::string jsonrpc;
    map<int, Json> queries;
    IWebSocketClient *wsc;
    uint lastId;
    ILogger *logger;

    int getNextId();

public:
    CJsonRpc(IWebSocketClient *wsc, ILogger *logger, JsonRpcParams params);
    Json request(Json jsonMap);
    virtual void handleMessage(const string &payload);
};
