#include "../src/polkadot.h"

int main(int argc, char *argv[]) {

    JsonRpcParams params;
    params.jsonrpc = "2.0";
    LoggerStub log;

    Json query = Json::object {
        {"method", "chain_getRuntimeVersion"},
        {"params", Json::array {} }
    };

    CJsonRpc jr(CWebSocketClient::getInstance(), &log, params);
    cout << "Message received(console app) " << jr.request(query).dump();

    delete CWebSocketClient::getInstance();
    return 0;
}
