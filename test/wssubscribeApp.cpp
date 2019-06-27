#include "../src/polkadot.h"

int main(int argc, char *argv[]) {

    // Create and connect app layer
    JsonRpcParams params;
    params.jsonrpcVersion = "2.0";

    EasyLogger logger;
    CJsonRpc jsonRpc(CWebSocketClient::getInstance(&logger), &logger, params);

    CPolkaApi app(&logger, &jsonRpc);
    app.connect();

    // Subscribe to block number updates
    bool done = false;
    app.subscribeBlockNumber([&](long long blockNum) {
        cout << endl << "Most recent block: " << blockNum << endl << endl;
        done = true;
    });

    // Wait until block number update arrives
    while (!done)
        usleep(10000);

    // Uncomment if you want to watch for more blocks
    // usleep(30000000);

    // Unsubscribe and close connection
    app.unsubscribeBlockNumber();
    app.disconnect();

    cout << "success" << endl;

    return 0;
}
