#include "../src/polkadot.h"

int main(int argc, char *argv[]) {

    JsonRpcParams params;
    params.jsonrpcVersion = "2.0";
    EasyLogger log;

    Json query = Json::object{{"method", "chain_getRuntimeVersion"}, {"params", Json::array{}}};

    CWebSocketClient::getInstance(&log)->connect();
    CJsonRpc jr(CWebSocketClient::getInstance(&log), &log, params);
    Json response = jr.request(query);

    cout << endl << "Message received(console app) " << response.dump() << endl;

    CWebSocketClient::getInstance(nullptr)->disconnect();
    cout << "success" << endl;

    delete CWebSocketClient::getInstance(nullptr);
    return 0;
}
