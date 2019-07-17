#include "../src/polkadot.h"
#include "helpers/cli.h"
#undef NDEBUG
#include <cassert>

int main(int argc, char *argv[]) {

    // auto app = polkadot::api::getInstance()->app();

    // app->connect();

    // CPolkaApi app(&logger, &jsonRpc);

    // string nodeUrl = getNodeUrlParam(argc, argv);
    // app.connect(nodeUrl);

    // auto resp2 = app.getSystemInfo();
    // assert(strlen(resp2->chainId) > 0);
    // assert(strlen(resp2->chainName) > 0);
    // assert(strlen(resp2->tokenSymbol) > 0);

    // cout << "Message received(connectApp - console app): " << endl
    //      << "  Chain ID       : " << resp2->chainId << endl
    //      << "  Chain Name     : " << resp2->chainName << endl
    //      << "  Token Decimals : " << resp2->tokenDecimals << endl
    //      << "  Token Symbol   : " << resp2->tokenSymbol << endl;

    // auto resp3 = app->getRuntimeVersion(nullptr);

    // cout << resp3->specName << endl;
    // for (auto item : resp3->api) {
    //     if (item.id)
    //         cout << "num:      " << item.num << "id:      " << item.id << endl;
    // }

    // app->disconnect();

    cout << "success" << endl;

    return 0;
}
