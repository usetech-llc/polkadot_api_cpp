#include "../src/polkadot.h"
#include "helpers/mockjsonrpc.h"
#undef NDEBUG
#include <cassert>

int main(int argc, char *argv[]) {

    auto app = polkadot::api::getInstance()->app();
    app->connect();

    cout << endl << endl << "============================ Get Health ============================" << endl;
    auto resp3 = app->getSystemHealth();

    cout << endl << endl << "Peer count         : " << resp3->peers << endl;
    cout << "Is synching        : " << (resp3->isSyncing ? "Yes" : "No") << endl;
    cout << "Should have peers  : " << (resp3->shouldHavePeers ? "Yes" : "No") << endl << endl;

    assert(resp3->peers > 0);

    app->disconnect();

    cout << "success" << endl;

    return 0;
}
