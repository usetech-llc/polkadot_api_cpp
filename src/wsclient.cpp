#include "polkadot.h"

CWebSocketClient *CWebSocketClient::_instance = nullptr;
chrono::seconds CWebSocketClient::ConnectionTimeout(5); // 5 second connection timeout

CWebSocketClient::CWebSocketClient(ILogger *logger)
    : _logger(logger), _connectedThread(nullptr), _healthThread(nullptr), _connected(false) {}

CWebSocketClient::~CWebSocketClient() {
    delete _connectedThread;
    delete _healthThread;
}

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
    inst->_connected = true;
    inst->_connectionCV.notify_all();
}

/// Verify that one of the subject alternative names matches the given hostname
bool verify_subject_alternative_name(const char *hostname, X509 *cert) {
    return true;

    STACK_OF(GENERAL_NAME) *san_names = NULL;

    san_names = (STACK_OF(GENERAL_NAME) *)X509_get_ext_d2i(cert, NID_subject_alt_name, NULL, NULL);
    if (san_names == NULL) {
        return false;
    }

    int san_names_count = sk_GENERAL_NAME_num(san_names);

    bool result = false;

    for (int i = 0; i < san_names_count; i++) {
        const GENERAL_NAME *current_name = sk_GENERAL_NAME_value(san_names, i);

        if (current_name->type != GEN_DNS) {
            continue;
        }

        char *dns_name = (char *)ASN1_STRING_data(current_name->d.dNSName);

        // Make sure there isn't an embedded NUL character in the DNS name
        if (ASN1_STRING_length(current_name->d.dNSName) != (int)strlen(dns_name)) {
            break;
        }
        // Compare expected hostname with the CN
        result = (strcasecmp(hostname, dns_name) == 0);
    }
    sk_GENERAL_NAME_pop_free(san_names, GENERAL_NAME_free);

    return result;
}

/// Verify that the certificate common name matches the given hostname
bool verify_common_name(const char *hostname, X509 *cert) {
    return true;

    // Find the position of the CN field in the Subject field of the certificate
    int common_name_loc = X509_NAME_get_index_by_NID(X509_get_subject_name(cert), NID_commonName, -1);
    if (common_name_loc < 0) {
        return false;
    }

    // Extract the CN field
    X509_NAME_ENTRY *common_name_entry = X509_NAME_get_entry(X509_get_subject_name(cert), common_name_loc);
    if (common_name_entry == NULL) {
        return false;
    }

    // Convert the CN field to a C string
    ASN1_STRING *common_name_asn1 = X509_NAME_ENTRY_get_data(common_name_entry);
    if (common_name_asn1 == NULL) {
        return false;
    }

    char *common_name_str = (char *)ASN1_STRING_data(common_name_asn1);

    // Make sure there isn't an embedded NUL character in the CN
    if (ASN1_STRING_length(common_name_asn1) != (int)strlen(common_name_str)) {
        return false;
    }

    // Compare expected hostname with the CN
    return (strcasecmp(hostname, common_name_str) == 0);
}

/**
 * This code is derived from examples and documentation found ato00po
 * http://www.boost.org/doc/libs/1_61_0/doc/html/boost_asio/example/cpp03/ssl/client.cpp
 * and
 * https://github.com/iSECPartners/ssl-conservatory
 */
bool verify_certificate(const char *hostname, bool preverified, boost::asio::ssl::verify_context &ctx) {
    // The verify callback can be used to check whether the certificate that is
    // being presented is valid for the peer. For example, RFC 2818 describes
    // the steps involved in doing this for HTTPS. Consult the OpenSSL
    // documentation for more details. Note that the callback is called once
    // for each certificate in the certificate chain, starting from the root
    // certificate authority.

    // Retrieve the depth of the current cert in the chain. 0 indicates the
    // actual server cert, upon which we will perform extra validation
    // (specifically, ensuring that the hostname matches. For other certs we
    // will use the 'preverified' flag from Asio, which incorporates a number of
    // non-implementation specific OpenSSL checking, such as the formatting of
    // certs and the trusted status based on the CA certs we imported earlier.
    int depth = X509_STORE_CTX_get_error_depth(ctx.native_handle());

    // if we are on the final cert and everything else checks out, ensure that
    // the hostname is present on the list of SANs or the common name (CN).
    if (depth == 0 && preverified) {
        X509 *cert = X509_STORE_CTX_get_current_cert(ctx.native_handle());

        if (verify_subject_alternative_name(hostname, cert)) {
            return true;
        } else if (verify_common_name(hostname, cert)) {
            return true;
        } else {
            return false;
        }
    }

    if (!preverified) {
        throw websocketpp::exception(X509_verify_cert_error_string(X509_STORE_CTX_get_error(ctx.native_handle())));
    }

    return preverified;
}

context_ptr on_tls_init(const char *hostname, websocketpp::connection_hdl) {
    context_ptr ctx = websocketpp::lib::make_shared<boost::asio::ssl::context>(boost::asio::ssl::context::sslv23);

    try {
        ctx->set_options(boost::asio::ssl::context::default_workarounds | boost::asio::ssl::context::no_sslv2 |
                         boost::asio::ssl::context::no_sslv3 | boost::asio::ssl::context::single_dh_use);

        ctx->set_verify_mode(boost::asio::ssl::verify_peer);
        ctx->set_verify_callback(bind(&verify_certificate, hostname, ::_1, ::_2));

        // Here we load the CA certificates of all CA's that this client trusts.
        ctx->load_verify_file(CConstants::certificate_file);
    } catch (std::exception &e) {
        CWebSocketClient *inst = (CWebSocketClient *)CWebSocketClient::getInstance(nullptr);
        inst->_logger->error(string("TLS Init Error. Trusted root CA certificate file: ") +
                             CConstants::certificate_file + ", exception message: " + e.what());
    }
    return ctx;
}

void CWebSocketClient::runWsMessages() {
    // will exit when this connection is closed.
    try {
        _tls ? _c.run() : _c_no_tls.run();
    } catch (websocketpp::exception const &e) {
        CWebSocketClient *inst = (CWebSocketClient *)CWebSocketClient::getInstance(nullptr);
        inst->_logger->error(string("Could not establish connection: ") + e.what());
        inst->_connected = false;
        inst->_connectionCV.notify_all();
    }
    _logger->info("runWsMessages Thread exited");
}

int CWebSocketClient::connect_tls(string node_url) {
    // Disable websocket++ logging, we have our own
    _c.set_access_channels(websocketpp::log::alevel::none);
    _c.set_error_channels(websocketpp::log::elevel::none);

    _c.set_open_handler(bind(&on_open, &_c, ::_1));

    // Initialize ASIO
    _c.init_asio();

    // Register our message handler
    _c.set_message_handler(&on_message);

    // Support wss
    _c.set_tls_init_handler(bind(&on_tls_init, node_url.c_str(), ::_1));

    websocketpp::lib::error_code ec;
    _connection = _c.get_connection(node_url, ec);
    if (ec) {
        _logger->error("Could not create connection: " + ec.message());
        return PAPI_CANT_CONNECT;
    }

    // Note that connect here only requests a connection. No network messages are
    // exchanged until the event loop starts running in the next line.
    _c.connect(_connection);

    return PAPI_OK;
}

int CWebSocketClient::connect_no_tls(string node_url) {
    // Disable websocket++ logging, we have our own
    _c_no_tls.set_access_channels(websocketpp::log::alevel::none);
    _c_no_tls.set_error_channels(websocketpp::log::elevel::none);

    _c_no_tls.set_open_handler(bind(&on_open, &_c, ::_1));

    // Initialize ASIO
    _c_no_tls.init_asio();

    // Register our message handler
    _c_no_tls.set_message_handler(&on_message);

    websocketpp::lib::error_code ec;
    _connection_no_tls = _c_no_tls.get_connection(node_url, ec);
    if (ec) {
        _logger->error("Could not create connection: " + ec.message());
        return PAPI_CANT_CONNECT;
    }

    // Note that connect here only requests a connection. No network messages are
    // exchanged until the event loop starts running in the next line.
    _c_no_tls.connect(_connection_no_tls);

    return PAPI_OK;
}

int CWebSocketClient::connect(string node_url) {

    std::string uri;
    if (node_url == "")
        uri = CConfig::getNodeUrl();
    else
        uri = node_url;

    try {
        _logger->info(string("Connecting to ") + uri);
        // Support both wss and ws
        int err;
        if (uri.substr(0, 4) == "wss:") {
            _tls = true;
            _logger->info("Connecting secure endpoint");
            err = connect_tls(uri);
        } else if (uri.substr(0, 3) == "ws:") {
            _tls = false;
            _logger->warning("Connecting to an insecure endpoint");
            err = connect_no_tls(uri);
        } else {
            _logger->error("Protocol is not specified in URL. Should begin with either 'ws:' or 'wss:'");
            return PAPI_CANT_CONNECT;
        }

        if (err != PAPI_OK)
            return err;

        // Start the ASIO io_service run loop
        // this will cause a single connection to be made to the server. c.run()
        std::unique_lock<std::mutex> connectionWaitLock(_connectionMtx);
        _connectedThread = new thread(&CWebSocketClient::runWsMessages, this);

        // Wait for connection
        _connectionCV.wait_for(connectionWaitLock, ConnectionTimeout);
        if (_connected) {
            _healthThread = new thread(&CWebSocketClient::health, this);
            _logger->info("Connection established");
        } else {
            _connectedThread->join();
            delete _connectedThread;
            _connectedThread = nullptr;
            _logger->error("Connection failed");
            return PAPI_CANT_CONNECT;
        }

    } catch (websocketpp::exception const &e) {
        _logger->error(string("Could not create connection: ") + e.what());
        return PAPI_CANT_CONNECT;
    }

    return PAPI_OK;
}

bool CWebSocketClient::isConnected() { return _connected; }

void CWebSocketClient::disconnect() {
    _tls ? _c.close(_connection, websocketpp::close::status::going_away, "")
         : _c_no_tls.close(_connection_no_tls, websocketpp::close::status::going_away, "");
    _connected = false;
    _connectedThread->join();
    if (_healthThread) {
        _healthThread->join();
    }
}

int CWebSocketClient::send(const string &msg) {
    if (_connected) {
        _sendMtx.lock();
        _tls ? _c.send(_connection, msg, websocketpp::frame::opcode::text)
             : _c_no_tls.send(_connection_no_tls, msg, websocketpp::frame::opcode::text);
        _sendMtx.unlock();
        _logger->info(string("WS sent message: ") + msg);
        return PAPI_OK;
    }

    return PAPI_NOT_CONNECTED;
}

void CWebSocketClient::health() {

    // hardcoded health message
    Json request = Json::object{
        {"id", INT_MAX}, {"jsonrpc", "2.0"}, {"method", "system_health"}, {"params", Json::array()},
    };

    long period_counter = 0;
    while (1) {
        usleep(HEALTH_WAKEUP_MKS);
        period_counter += HEALTH_WAKEUP_MKS;
        if (!isConnected())
            break;
        if (_connectedThread == nullptr)
            break;

        if (period_counter >= CConstants::health_check_delay_time) {
            send(request.dump());
            period_counter = 0;
        }
    }
}

void CWebSocketClient::registerMessageObserver(IMessageObserver *handler) { _observers.push_back(handler); }
