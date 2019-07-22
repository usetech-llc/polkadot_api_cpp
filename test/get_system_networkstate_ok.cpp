#include "../src/polkadot.h"
#include "helpers/mockjsonrpc.h"
#undef NDEBUG
#include <cassert>

int main(int argc, char *argv[]) {
 
    auto app = polkadot::api::getInstance()->app();
    app->connect();

    auto resp3 = app->getNetworkState();

    assert(strcmp(resp3->connectedPeers->key, "") != 0);
    assert(strcmp(resp3->peerId, "") != 0);

    app->disconnect();

    cout << "success" << endl;

    return 0;
}
