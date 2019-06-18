struct JsonRpcParams {
    string jsonrpc;
};

struct JsonRpcQuery {
    int id;
    Json json;
    condition_variable *cv;
    mutex *m;
};

class JsonRpcException : public exception {
    string _msg;

public:
    JsonRpcException(string msg) { _msg = msg; };
    virtual char const *what() const noexcept { return _msg.c_str(); }
};

class CJsonRpc : public IMessageObserver, public IJsonRpc {
private:
    std::string jsonrpc;
    IWebSocketClient *_wsc;
    uint lastId;
    ILogger *logger;

    // Map between request IDs and waiting requests
    map<int, JsonRpcQuery> _queries;
    mutex _queryMtx;

    // Map between subscription IDs and subscribers
    map<int, IWebSocketMessageObserver *> _wsSubscribers;

    int getNextId();

public:
    virtual ~CJsonRpc() override {}
    virtual int connect();
    virtual void disconnect();
    CJsonRpc(IWebSocketClient *wsc, ILogger *logger, JsonRpcParams params);
    virtual Json request(Json jsonMap);
    virtual void handleMessage(const string &payload);
    virtual int subscribeWs(Json jsonMap, IWebSocketMessageObserver *observer);
    virtual int unsubscribeWs(Json jsonMap, IWebSocketMessageObserver *observer);
};
