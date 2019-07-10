#include "../src/polkadot.h"

int main(int argc, char *argv[]) {

  JsonRpcParams params;
    params.jsonrpcVersion = "2.0";

    EasyLogger logger;
    CJsonRpc jsonRpc(CWebSocketClient::getInstance(&logger), &logger, params);

    CPolkaApi app(&logger, &jsonRpc);
    app.connect();

    // Subscribe to account nonce updates
    string addr("5ECcjykmdAQK71qHBCkEWpWkoMJY6NXvpdKy8UeMx16q5gFr");
    bool done = false;
    app.subscribeAccountNonce(addr, [&](unsigned long nonce) {
        cout << endl << "  Account Nonce: " << to_string(nonce) << endl << endl;
        done = true;
    });

    // Wait until block number update arrives
    while (!done)
        usleep(10000);

    // Uncomment if you want to watch for more updates
    // usleep(30000000);

    // Unsubscribe and close connection
    app.unsubscribeAccountNonce(addr);
    app.disconnect();

    cout << "success" << endl;

    return 0;
}
