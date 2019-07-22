#include "../src/polkadot.h"
#include "helpers/mockjsonrpc.h"
#undef NDEBUG
#include <cassert>

int main(int argc, char *argv[]) {

    auto app = polkadot::api::getInstance()->app();
    app->connect();

    cout << endl << endl << "============================ Get Peers ============================" << endl;
    auto resp3 = app->getSystemPeers();

    cout << endl << endl;
    cout << "Peer count: " << resp3->count << endl;
    for (auto i = 0; i < resp3->count; i++) {
        cout << "Peer " << i << ", ID: " << resp3->peers[i].peerId << ", best block: " << resp3->peers[i].bestNumber
             << ", Protocol: " << resp3->peers[i].protocolVersion << ", Roles: " << resp3->peers[i].roles << endl;

        assert(strlen(resp3->peers[i].peerId) > 0);
        assert(resp3->peers[i].bestNumber > 0);
        assert(resp3->peers[i].protocolVersion > 0);
    }
    cout << endl << endl;

    app->disconnect();

    cout << "success" << endl;

    return 0;
}
