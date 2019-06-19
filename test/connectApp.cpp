#include "../src/polkadot.h"

int main(int argc, char *argv[]) {

    CPolkaApi app;

    string blockHash = "0xb72a241d0a3a38268671caa3bc4ad7e88015cff341a1da0f1982227c5a52a3e3";
   // GetRuntimeVersionParams par;
   // par.

    unique_ptr<GetRuntimeVersionParams> par(new GetRuntimeVersionParams);
    strcpy(par->blockHash, blockHash.c_str());

    app.connect();
    // auto resp = move(app.getSystemInfo());
    // cout << endl << "Message received(console app) " << resp->chainName << endl;
    auto resp = move(app.getRuntimeVersion(move(par)));
    // for(auto item :resp->api)
    // {
    //     cout << "num:      " << item.num << "id:      " << item.id << endl;
    // }

    app.disconnect();


    // JsonRpcParams params;
    // params.jsonrpcVersion = "2.0";
    // LoggerStub log;

    // Json query = Json::object{{"method", "chain_getRuntimeVersion"}, {"params", Json::array{}}};

    // CWebSocketClient::getInstance(&log)->connect();
    // CJsonRpc jr(CWebSocketClient::getInstance(&log), &log, params);
    // Json response = jr.request(query);

    // cout << endl << "Message received(console app) " << response.dump() << endl;


    cout << "success" << endl;

    delete CWebSocketClient::getInstance(nullptr);
    return 0;
}
