typedef websocketpp::client<websocketpp::config::asio_tls_client> client;
typedef websocketpp::client<websocketpp::config::asio_client> client_no_tls;
typedef websocketpp::lib::shared_ptr<websocketpp::lib::asio::ssl::context> context_ptr;

#define HEALTH_WAKEUP_MKS 100000

class CWebSocketClient : public IWebSocketClient {
private:
    ILogger *_logger;
    static CWebSocketClient *_instance;
    vector<IMessageObserver *> _observers;
    thread *_connectedThread;
    thread *_healthThread;
    client _c;
    client_no_tls _c_no_tls;
    bool _tls;
    string _node_url;
    client::connection_ptr _connection;
    client_no_tls::connection_ptr _connection_no_tls;
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
    int connect_tls(string node_url);
    int connect_no_tls(string node_url);

    bool compare_host_name(const char *cert_hostname);

    static void on_message(websocketpp::connection_hdl, client::message_ptr msg);
    static void on_open(client *c, websocketpp::connection_hdl hdl);
    static bool verify_subject_alternative_name(const char *hostname, X509 *cert);
    static bool verify_common_name(const char *hostname, X509 *cert);
    static bool verify_certificate(const char *hostname, bool preverified, boost::asio::ssl::verify_context &ctx);
    static context_ptr on_tls_init(const char *hostname, websocketpp::connection_hdl);

    CWebSocketClient(ILogger *logger);

public:
    static IWebSocketClient *getInstance(ILogger *logger);
    ~CWebSocketClient() override;

    virtual int connect(string node_url = "");
    virtual bool isConnected();
    virtual void disconnect();
    virtual int send(const string &msg);

    virtual void registerMessageObserver(IMessageObserver *handler);
};
