#include "../src/polkadot.h"
#include "helpers/cli.h"
#undef NDEBUG
#include <cassert>

int main(int argc, char *argv[]) {

    // Create and connect app layer
    JsonRpcParams params;
    params.jsonrpcVersion = "2.0";

    EasyLogger logger;
    CJsonRpc jsonRpc(CWebSocketClient::getInstance(&logger), &logger, params);

    CPolkaApi app(&logger, &jsonRpc);
    app.connect(getNodeUrlParam(argc, argv));

    // Subscribe to block number updates
    bool done = false;
    long long blockNumber = 0;
    app.subscribeBlockNumber([&](long long blockNum) {
        cout << endl << "Most recent block: " << blockNum << endl << endl;
        blockNumber = blockNum;
        done = true;
    });

    // Wait until block number update arrives
    while (!done)
        usleep(10000);

    assert(blockNumber > 0);

    // Uncomment if you want to watch for more blocks
    // usleep(300000000);

    // Unsubscribe and close connection
    app.unsubscribeBlockNumber();
    app.disconnect();

    cout << "success" << endl;

    return 0;
}
