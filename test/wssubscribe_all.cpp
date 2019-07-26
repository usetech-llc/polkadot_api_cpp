#include "../src/polkadot.h"
#include "helpers/cli.h"
#undef NDEBUG
#include <cassert>

int main(int argc, char *argv[]) {

    auto app = polkadot::api::getInstance()->app();
    app->connect(getNodeUrlParam(argc, argv));

    // Subscribe to finalized block updates
    bool doneBlock = false;
    long long blockNumber = 0;
    app->subscribeFinalizedBlock([&](const BlockHeader &header) {
        cout << endl << "Most recent block: " << header.number << endl << endl;
        blockNumber = header.number;
        doneBlock = true;
    });

    // Subscribe to runtime version updates
    bool doneRT = false;
    string specName;
    app->subscribeRuntimeVersion([&](const RuntimeVersion &rtv) {
        cout << endl << "Runtime version spec name: " << rtv.specName << endl << endl;
        specName = rtv.specName;
        doneRT = true;
    });

    // Subscribe to storage updates (timestamp)
    string module = "Timestamp";
    string variable = "Now";
    string key = app->getKeys("", module, variable);
    bool doneS = false;
    string tsUpdate;
    app->subscribeStorage(key, [&](const string &update) {
        cout << endl << "Timestamp update: " << update << endl << endl;
        tsUpdate = update;
        doneS = true;
    });

    // Wait until all updates arrive
    while (!(doneBlock && doneRT && doneS))
        usleep(10000);

    assert(blockNumber > 0);
    assert(specName.length() > 0);
    assert(tsUpdate.length() > 0);
    assert(tsUpdate.substr(0, 2) == "0x");

    // Unsubscribe and close connection
    app->unsubscribeFinalizedBlock();
    app->unsubscribeRuntimeVersion();
    app->unsubscribeStorage(key);
    app->disconnect();

    cout << "success" << endl;

    return 0;
}
