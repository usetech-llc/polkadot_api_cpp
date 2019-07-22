#include "../src/polkadot.h"
#include "helpers/mockjsonrpc.h"
#undef NDEBUG
#include <cassert>

int main(int argc, char *argv[]) {
 
    auto app = polkadot::api::getInstance()->app();
    app->connect();

    auto resp3 = app->getSystemPeers();

    for(auto i = 0; i < resp3->count; i++){
        assert(resp3->peers[i].protocolVersion > 0);
    }

    app->disconnect();

    cout << "success" << endl;

    return 0;
}
