#include "../src/polkadot.h"
#include "helpers/cli.h"
#undef NDEBUG
#include <cassert>

int main(int argc, char *argv[]) {

    JsonRpcParams params;
    params.jsonrpcVersion = "2.0";
    EasyLogger log;

    Json query = Json::object{{"method", "chain_getRuntimeVersion"}, {"params", Json::array{}}};

    string nodeUrl = getNodeUrlParam(argc, argv);
    CWebSocketClient::getInstance(&log)->connect(nodeUrl);
    CJsonRpc jr(CWebSocketClient::getInstance(&log), &log, params);
    Json response = jr.request(query);

    assert(response.dump().length() > 10);
    cout << endl << "Message received(console app) " << response.dump() << endl;

    CWebSocketClient::getInstance(nullptr)->disconnect();
    cout << "success" << endl;

    delete CWebSocketClient::getInstance(nullptr);
    return 0;
}
