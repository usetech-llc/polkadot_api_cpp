#include "polkadot.h"

CWebSocketClient *CWebSocketClient::_instance = nullptr;
chrono::seconds CWebSocketClient::ConnectionTimeout(5); // 5 second connection timeout

CWebSocketClient::CWebSocketClient(ILogger *logger)
    : _nodeUrl(CConstants::parity_node_url), _logger(logger), _connectedThread(nullptr), _connected(false) {}

CWebSocketClient::~CWebSocketClient() { delete _connectedThread; }

IWebSocketClient *CWebSocketClient::getInstance(ILogger *logger) {
    if (!CWebSocketClient::_instance) {
        CWebSocketClient::_instance = new CWebSocketClient(logger);
    }
    return (IWebSocketClient *)CWebSocketClient::_instance;
}

using websocketpp::lib::bind;
using websocketpp::lib::placeholders::_1;
using websocketpp::lib::placeholders::_2;

void on_message(websocketpp::connection_hdl, client::message_ptr msg) {
    if (msg->get_opcode() == websocketpp::frame::opcode::text) {
        // Notify observers
        CWebSocketClient *inst = (CWebSocketClient *)CWebSocketClient::getInstance(nullptr);
        string payload(msg->get_payload());
        inst->_logger->info(string("WS Received Message: ") + payload);
        for (auto o : inst->_observers) {
            o->handleMessage(payload);
        }
    }
}

void on_open(client *c, websocketpp::connection_hdl hdl) {
    CWebSocketClient *inst = (CWebSocketClient *)CWebSocketClient::getInstance(nullptr);
    inst->_connectionCV.notify_all();
}

bool verify_certificate(const char *hostname, bool preverified, boost::asio::ssl::verify_context &ctx) { return true; }

context_ptr on_tls_init(const char *hostname, websocketpp::connection_hdl) {
    context_ptr ctx = websocketpp::lib::make_shared<boost::asio::ssl::context>(boost::asio::ssl::context::sslv23);

    try {
        ctx->set_options(boost::asio::ssl::context::default_workarounds | boost::asio::ssl::context::no_sslv2 |
                         boost::asio::ssl::context::no_sslv3 | boost::asio::ssl::context::single_dh_use);

        ctx->set_verify_mode(boost::asio::ssl::verify_peer);
        ctx->set_verify_callback(bind(&verify_certificate, hostname, ::_1, ::_2));

        // Here we load the CA certificates of all CA's that this client trusts.
        ctx->load_verify_file("ca-chain.cert.pem");
    } catch (std::exception &e) {
        CWebSocketClient *inst = (CWebSocketClient *)CWebSocketClient::getInstance(nullptr);
        inst->_logger->error(string("TLS Init Error: ") + e.what());
    }
    return ctx;
}

void CWebSocketClient::runWsMessages() {
    // will exit when this connection is closed.
    _c.run();
    _logger->info("runWsMessages Thread exited");
}

int CWebSocketClient::connect() {

    std::string uri = _nodeUrl;

    try {
        // Disable websocket++ logging, we have our own
        _c.set_access_channels(websocketpp::log::alevel::none);
        _c.set_error_channels(websocketpp::log::elevel::none);

        _c.set_open_handler(bind(&on_open, &_c, ::_1));

        // Initialize ASIO
        _c.init_asio();

        // Register our message handler
        _c.set_message_handler(&on_message);
        _c.set_tls_init_handler(bind(&on_tls_init, uri.c_str(), ::_1));

        websocketpp::lib::error_code ec;
        _connection = _c.get_connection(uri, ec);
        if (ec) {
            _logger->error("Could not create connection: " + ec.message());
            return PAPI_CANT_CONNECT;
        }

        // Note that connect here only requests a connection. No network messages are
        // exchanged until the event loop starts running in the next line.
        _c.connect(_connection);

        _logger->info(string("Connecting to ") + uri);

        // Start the ASIO io_service run loop
        // this will cause a single connection to be made to the server. c.run()
        std::unique_lock<std::mutex> connectionWaitLock(_connectionMtx);
        _connectedThread = new thread(&CWebSocketClient::runWsMessages, this);

        // Wait for connection
        _connectionCV.wait_for(connectionWaitLock, ConnectionTimeout);
        _connected = true;
        _logger->info("Connection established");

    } catch (websocketpp::exception const &e) {
        _logger->error(string("Could not create connection: ") + e.what());
        return PAPI_CANT_CONNECT;
    }

    return 0;
}

bool CWebSocketClient::isConnected() { return _connected; }

void CWebSocketClient::disconnect() {
    _c.close(_connection, websocketpp::close::status::going_away, "");
    _connectedThread->join();
    _connected = false;
}

int CWebSocketClient::send(const string &msg) {
    _c.send(_connection, msg, websocketpp::frame::opcode::text);
    _logger->info(string("WS Sent Message: ") + msg);

    return 0;
}

void CWebSocketClient::registerMessageObserver(IMessageObserver *handler) { _observers.push_back(handler); }
