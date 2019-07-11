typedef websocketpp::client<websocketpp::config::asio_tls_client> client;
typedef websocketpp::lib::shared_ptr<websocketpp::lib::asio::ssl::context> context_ptr;

#define HEALTH_WAKEUP_MKS 100000

class CWebSocketClient : public IWebSocketClient {
private:
    string _nodeUrl;
    ILogger *_logger;
    static CWebSocketClient *_instance;
    vector<IMessageObserver *> _observers;
    thread *_connectedThread;
    thread *_healthThread;
    client _c;
    client::connection_ptr _connection;
    bool _connected;
    condition_variable _connectionCV; // Condition variable used to notify about connection
    mutex _connectionMtx;             // Mutex for condition varaiable
    static chrono::seconds ConnectionTimeout;
    mutex _sendMtx; // Mutex for sending data to be block send method

    friend context_ptr on_tls_init(const char *hostname, websocketpp::connection_hdl);
    friend void on_message(websocketpp::connection_hdl, client::message_ptr msg);
    friend void on_open(client *c, websocketpp::connection_hdl hdl);
    void health();
    void runWsMessages();

    CWebSocketClient(ILogger *logger);

public:
    static IWebSocketClient *getInstance(ILogger *logger);
    ~CWebSocketClient() override;

    virtual int connect();
    virtual bool isConnected();
    virtual void disconnect();
    virtual int send(const string &msg);

    virtual void registerMessageObserver(IMessageObserver *handler);
};
