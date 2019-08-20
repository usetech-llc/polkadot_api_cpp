struct JsonRpcParams {
    string jsonrpcVersion;
};

struct JsonRpcQuery {
    int id;
    Json json;
    condition_variable *completionCV;
    mutex *completionMtx;
};

class JsonRpcException : public exception {
    string _msg;

public:
    JsonRpcException(string msg) { _msg = msg; };
    virtual char const *what() const noexcept { return _msg.c_str(); }
};

class CJsonRpc : public IMessageObserver, public IJsonRpc {
private:
    std::string _jsonrpcVersion;
    unsigned int _lastId;
    ILogger *_logger;
    IWebSocketClient *_wsc;

    // Map between request IDs and waiting requests
    map<int, JsonRpcQuery> _queries;
    mutex _queryMtx;

    // Map between subscription IDs and subscribers
    map<int, IWebSocketMessageObserver *> _wsSubscribers;

    void delayedUpdateThread(Json message, int subscriptionId);

    int getNextId();

public:
    CJsonRpc(IWebSocketClient *wsc, ILogger *logger, JsonRpcParams params);
    virtual ~CJsonRpc() override {}
    virtual int connect(string node_url = "");
    virtual void disconnect();
    virtual Json request(Json jsonMap, long timeout_s = RESPONSE_TIMEOUT_S);
    virtual void handleMessage(const string &payload);
    virtual int subscribeWs(Json jsonMap, IWebSocketMessageObserver *observer);
    virtual int unsubscribeWs(int subscriptionId, string method);
};
