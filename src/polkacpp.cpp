// clang-format off
#include <websocketpp/client.hpp>
#include <websocketpp/config/asio_client.hpp>
#include <websocketpp/config/asio_no_tls_client.hpp>

#include "libs/int128/int128.h"
typedef absl::uint128 uint128;

#include "libs/easyloggingpp/easylogging++.h"
#include "libs/json11/json11.hpp"
#include "scale.h"
using namespace json11;

extern "C" {
    #include "utils/sr25519.h"
}

#include "polkacpp.h"

#include <condition_variable>

#include "structs/publickey.h"
#include "structs/hasher.h"
#include "structs/signaturepayload.h"

#include "interfaces/ilogger.h"
#include "interfaces/imessageobserver.h"
#include "interfaces/iwsmessageobserver.h"
#include "interfaces/iwsclient.h"
#include "interfaces/ijsonrpc.h"

#include "config.h"
#include "wsclient.h"
#include "elog.h"
#include "application.h"

#include "jsonrpc.h"
// clang-format on

using namespace std;

// Implementation of make_unique
template <typename T, typename... Args> std::unique_ptr<T> make_unique(Args &&... args) {
    return std::unique_ptr<T>(new T(std::forward<Args>(args)...));
}

class NullLogger : public ILogger {
public:
    virtual ~NullLogger() {}
    virtual void info(string message) {}
    virtual void error(string message) {}
    virtual void warning(string message) {}
};

class polkadot::api::impl {
    ILogger *_logger;
    IJsonRpc *_jsonRpc;
    IApplication *_app;

public:
    impl() {
        JsonRpcParams params;
        params.jsonrpcVersion = "2.0";

        _logger = new EasyLogger();
        _jsonRpc = new CJsonRpc(CWebSocketClient::getInstance(_logger), _logger, params);
        _app = new CPolkaApi(_logger, _jsonRpc);
    }
    ~impl() {
        delete _app;
        delete _jsonRpc;
        delete _logger;
        delete CWebSocketClient::getInstance(nullptr);
    }
    IApplication *app() { return _app; }
};

shared_ptr<polkadot::api> polkadot::api::_instance;
polkadot::api::api() : pImpl{make_unique<impl>()} {}
polkadot::api::~api() = default;
shared_ptr<polkadot::api> polkadot::api::getInstance() {
    if (!_instance)
        _instance = shared_ptr<polkadot::api>(new polkadot::api());
    return _instance;
}
IApplication *polkadot::api::app() { return pImpl->app(); }
