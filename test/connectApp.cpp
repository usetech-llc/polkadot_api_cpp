#include "../src/polkadot.h"

int main(int argc, char *argv[]) {

    CPolkaApi app;

   // string blockHash = "0xb72a241d0a3a38268671caa3bc4ad7e88015cff341a1da0f1982227c5a52a3e3";
   // GetRuntimeVersionParams par;
   // par.

    unique_ptr<GetBlockHashParams> par(new GetBlockHashParams);
    par->blockNumber = 2;

    app.connect();
    auto resp = app.getBlockHash(move(par));
    cout << "Message received(getBlockHash - console app) " << resp->hash << endl;

    auto resp2 = app.getSystemInfo();
    cout << "Message received(getBlockHash - console app) " << 
        resp2->chainId << endl << resp2->chainName << endl << 
        resp2->tokenDecimals << endl << resp2->tokenSymbol << endl;

    unique_ptr<GetRuntimeVersionParams> par2(new GetRuntimeVersionParams);
    strcpy(par2->blockHash, resp->hash);

    auto resp3 = app.getRuntimeVersion(move(par2));

    cout << resp3->specName << endl;
    for(auto item :resp3->api)
    {
        cout << "num:      " << item.num << "id:      " << item.id << endl;
    }

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
