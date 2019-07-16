class IWebSocketClient {
public:
    virtual ~IWebSocketClient(){};

    virtual int connect(string node_url = "") = 0;
    virtual bool isConnected() = 0;
    virtual void disconnect() = 0;
    virtual int send(const string &msg) = 0;

    virtual void registerMessageObserver(IMessageObserver *handler) = 0;
};
