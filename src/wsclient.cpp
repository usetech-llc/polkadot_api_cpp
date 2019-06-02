#include "polkadot.h"





CWebSocketClient::CWebSocketClient(string nodeUrl) :
    _nodeUrl(nodeUrl)
{
}

CWebSocketClient::CWebSocketClient()
{
}


typedef websocketpp::client<websocketpp::config::asio_tls_client> client;
typedef websocketpp::lib::shared_ptr<websocketpp::lib::asio::ssl::context> context_ptr;

using websocketpp::lib::placeholders::_1;
using websocketpp::lib::placeholders::_2;
using websocketpp::lib::bind;

void on_message(websocketpp::connection_hdl, client::message_ptr msg) {
    std::cout << msg->get_payload() << std::endl;
}

void on_open(client* c, websocketpp::connection_hdl hdl) {
    std::string msg = "{\"id\":2,\"jsonrpc\":\"2.0\",\"method\":\"chain_getRuntimeVersion\",\"params\":[]}";;
    c->send(hdl,msg,websocketpp::frame::opcode::text);
    c->get_alog().write(websocketpp::log::alevel::app, "Sent Message: "+msg);
}

bool verify_certificate(const char * hostname, bool preverified, boost::asio::ssl::verify_context& ctx) {
    return true;
}

context_ptr on_tls_init(const char * hostname, websocketpp::connection_hdl) {
    context_ptr ctx = websocketpp::lib::make_shared<boost::asio::ssl::context>(boost::asio::ssl::context::sslv23);

    try {
        ctx->set_options(boost::asio::ssl::context::default_workarounds |
                         boost::asio::ssl::context::no_sslv2 |
                         boost::asio::ssl::context::no_sslv3 |
                         boost::asio::ssl::context::single_dh_use);


        ctx->set_verify_mode(boost::asio::ssl::verify_peer);
        ctx->set_verify_callback(bind(&verify_certificate, hostname, ::_1, ::_2));

        // Here we load the CA certificates of all CA's that this client trusts.
        ctx->load_verify_file("ca-chain.cert.pem");
    } catch (std::exception& e) {
        std::cout << e.what() << std::endl;
    }
    return ctx;
}


//{"jsonrpc":"2.0","method":"state_storage","params":{"result":{"block":"0x4a10c0e96359e56170dee4ab0163d254a0adbf92dcaae242d4cdb391a42fb4be","changes":[["0xcc956bdb7605e3547539f321ac2bc95c","0x080000000000000000010000000000"]]},"subscription":3227192}}

int CWebSocketClient::connect()
{

    client c;

    std::string hostname = "poc3-rpc.polkadot.io";
    std::string port = "443";

    std::string uri = "wss://" + hostname + ":" + port;

    try {
        // Set logging to be pretty verbose (everything except message payloads)
        c.set_access_channels(websocketpp::log::alevel::all);
        c.clear_access_channels(websocketpp::log::alevel::frame_payload);
        c.set_error_channels(websocketpp::log::elevel::all);

        c.set_open_handler(bind(&on_open,&c,::_1));

        // Initialize ASIO
        c.init_asio();

        // Register our message handler
        c.set_message_handler(&on_message);
        c.set_tls_init_handler(bind(&on_tls_init, hostname.c_str(), ::_1));

        websocketpp::lib::error_code ec;
        client::connection_ptr con = c.get_connection(uri, ec);
        if (ec) {
            std::cout << "could not create connection because: " << ec.message() << std::endl;
            return 0;
        }

        // Note that connect here only requests a connection. No network messages are
        // exchanged until the event loop starts running in the next line.
        c.connect(con);

        c.get_alog().write(websocketpp::log::alevel::app, "Connecting to " + uri);

        //websocketpp::lib::error_code wsec;
        //std::string message = "{\"id\":2,\"jsonrpc\":\"2.0\",\"method\":\"chain_getRuntimeVersion\",\"params\":[]}";
        //c.send(con, message, websocketpp::frame::opcode::text, wsec);

        //c.poll(con, );



        // Start the ASIO io_service run loop
        // this will cause a single connection to be made to the server. c.run()
        // will exit when this connection is closed.
        c.run();
    } catch (websocketpp::exception const & e) {
        std::cout << e.what() << std::endl;
    }





    return 0;
}
