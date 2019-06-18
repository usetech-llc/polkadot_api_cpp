typedef websocketpp::client<websocketpp::config::asio_tls_client> client;
typedef websocketpp::lib::shared_ptr<websocketpp::lib::asio::ssl::context> context_ptr;

class CWebSocketClient : public IWebSocketClient {
private:
    string _nodeUrl;
    static CWebSocketClient *_instance;
    vector<IMessageObserver *> _observers;
    thread *_connectedThread;
    client _c;
    client::connection_ptr _connection;
    bool _connected;

    friend void on_message(websocketpp::connection_hdl, client::message_ptr msg);
    friend void on_open(client *c, websocketpp::connection_hdl hdl);
    void runWsMessages();

    CWebSocketClient();

public:
    static IWebSocketClient *getInstance();
    ~CWebSocketClient() override;

    virtual int connect();
    virtual bool isConnected();
    virtual void disconnect();
    virtual int send(const string &msg);

    virtual void registerMessageObserver(IMessageObserver *handler);
};
