#include "../src/polkadot.h"

int main(int argc, char *argv[]) {

    JsonRpcParams params;
    params.jsonrpcVersion = "2.0";
    LoggerStub log;

    Json query = Json::object{{"method", "chain_getRuntimeVersion"}, {"params", Json::array{}}};

    CWebSocketClient::getInstance()->connect();
    usleep(2000000);
    CJsonRpc jr(CWebSocketClient::getInstance(), &log, params);
    Json response = jr.request(query);

    cout << endl << "Message received(console app) " << response.dump() << endl;

    CWebSocketClient::getInstance()->disconnect();
    cout << "success" << endl;

    delete CWebSocketClient::getInstance();
    return 0;
}
