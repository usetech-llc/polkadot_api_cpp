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
    bool isEpoch = false;
    int eraLength = -1;
    int eraProgress = -1;
    int sessionLength = -1;
    int sessionProgress = -1;
    int epochLength = -1;
    int epochProgress = -1;
    app.subscribeEraAndSession([&](Era era, SessionOrEpoch session) {
        if (session.isEpoch) {
            cout << endl << "Epoch: " << session.epochProgress << " / " << session.epochLength << endl;
        } else {
            cout << endl << "Session: " << session.sessionProgress << " / " << session.sessionLength << endl;
        }
        cout << "Era: " << era.eraProgress << " / " << era.eraLength << endl << endl;

        eraLength = era.eraLength;
        eraProgress = era.eraProgress;
        isEpoch = session.isEpoch;

        if (session.isEpoch) {
            epochLength = session.epochLength;
            epochProgress = session.epochProgress;
        } else {
            sessionLength = session.sessionLength;
            sessionProgress = session.sessionProgress;
        }

        done = true;
    });

    // Wait until block number update arrives
    while (!done)
        usleep(2000);

    assert(eraLength > 0);
    assert(eraProgress >= 0);

    if (isEpoch) {
        assert(epochLength > 0);
        assert(epochProgress >= 0);
    } else {
        assert(sessionLength > 0);
        assert(sessionProgress >= 0);
    }

    // Unsubscribe and close connection
    app.unsubscribeEraAndSession();
    app.disconnect();

    cout << "success" << endl;

    return 0;
}
