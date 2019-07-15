#include "../src/polkadot.h"

int main(int argc, char *argv[]) {

    JsonRpcParams params;
    params.jsonrpcVersion = "2.0";

    EasyLogger logger;
    CJsonRpc jsonRpc(CWebSocketClient::getInstance(&logger), &logger, params);

    CPolkaApi app(&logger, &jsonRpc);

    app.connect();

    auto resp2 = app.getSystemInfo();
    cout << "Message received(connectApp - console app): " << endl
         << "  Chain ID       : " << resp2->chainId << endl
         << "  Chain Name     : " << resp2->chainName << endl
         << "  Token Decimals : " << resp2->tokenDecimals << endl
         << "  Token Symbol   : " << resp2->tokenSymbol << endl;

    auto resp3 = app.getRuntimeVersion(nullptr);

    cout << resp3->specName << endl;
    for (auto item : resp3->api) {
        if (item.id)
            cout << "num:      " << item.num << "id:      " << item.id << endl;
    }

    app.disconnect();

    cout << "success" << endl;

    delete CWebSocketClient::getInstance(nullptr);
    return 0;
}
