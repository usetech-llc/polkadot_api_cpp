#include "../src/polkadot.h"

int main(int argc, char *argv[]) {

    // This is a manual test due to lack of test DOTs
    if (argc < 4) {
        cout << "Usage: ";
        cout << argv[0] << " <sender address> <recipient address> <amount in fDOTs> <sender private key (hex)>" << endl;
        cout << "success" << endl;
        return 0;
    }

    // Extract parameters
    string senderAddr(argv[1]);
    string recipientAddr(argv[2]);
    string amountStr(argv[3]);
    string senderPrivateKeyStr(argv[4]);

    JsonRpcParams params;
    params.jsonrpcVersion = "2.0";

    EasyLogger logger;
    CJsonRpc jsonRpc(CWebSocketClient::getInstance(&logger), &logger, params);

    CPolkaApi app(&logger, &jsonRpc);
    app.connect();

    // Subscribe to account nonce updates
    bool done = false;
    app.signAndSendTransfer(senderAddr, senderPrivateKeyStr, recipientAddr, 1000000000000, [&](string result) {
        if (result == "ready")
            cout << endl << endl << "   ---=== Transaction was registered in network ===--- " << endl << endl << endl;
        if (result == "finalized") {
            cout << endl << endl << "   ---=== Transaction was mined! ===--- " << endl << endl << endl;
            done = true;
        }
    });

    // Wait until transaction is mined
    while (!done)
        usleep(10000);

    // Unsubscribe and close connection
    app.disconnect();

    cout << "success" << endl;

    return 0;
}
