#include "../src/polkadot.h"

int main(int argc, char *argv[]) {

    JsonRpcParams params;
    params.jsonrpcVersion = "2.0";

    LoggerStub logger;
    CJsonRpc jsonRpc(CWebSocketClient::getInstance(&logger), &logger, params);

    CPolkaApi app(&logger, &jsonRpc);

    // string blockHash = "0xb72a241d0a3a38268671caa3bc4ad7e88015cff341a1da0f1982227c5a52a3e3";
    // GetRuntimeVersionParams par;
    // par.

    unique_ptr<GetBlockHashParams> par(new GetBlockHashParams);
    par->blockNumber = 2;

    app.connect();
    auto resp = app.getBlockHash(move(par));
    cout << "Message received(getBlockHash - console app) " << resp->hash << endl;

    auto resp2 = app.getSystemInfo();
    cout << "Message received(getBlockHash - console app) " << resp2->chainId << endl
         << resp2->chainName << endl
         << resp2->tokenDecimals << endl
         << resp2->tokenSymbol << endl;

    unique_ptr<GetRuntimeVersionParams> par2(new GetRuntimeVersionParams);
    strcpy(par2->blockHash, resp->hash);

    auto resp3 = app.getRuntimeVersion(move(par2));

    cout << resp3->specName << endl;
    for (auto item : resp3->api) {
        cout << "num:      " << item.num << "id:      " << item.id << endl;
    }

    app.disconnect();

    cout << "success" << endl;

    delete CWebSocketClient::getInstance(nullptr);
    return 0;
}
