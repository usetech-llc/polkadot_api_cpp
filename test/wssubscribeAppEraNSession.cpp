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
    int currentEra = -1;
    int sessionIndex = -1;
    int eraProgress = -1;
    int sessionProgress = -1;
    app.subscribeEraAndSession([&](Era era, Session session) {
        cout << endl << "currentEra: " << era.currentEra << endl << endl;
        cout << endl << "eraProgress: " << era.eraProgress << endl << endl;
        cout << endl << "sessionIndex: " << session.sessionIndex << endl << endl;
        cout << endl << "sessionProgress: " << session.sessionProgress << endl << endl;

        currentEra = era.currentEra;
        sessionIndex = session.sessionIndex;
        eraProgress = era.eraProgress;
        sessionProgress = session.sessionProgress;

        done = true;
    });

    // Wait until block number update arrives
    while (!done)
        usleep(2000);

    assert(currentEra > 0);
    assert(sessionIndex > 0);
    assert(eraProgress > 0);
    assert(sessionProgress > 0);

    // Unsubscribe and close connection
    app.unsubscribeEraAndSession();
    app.disconnect();

    cout << "success" << endl;

    return 0;
}
