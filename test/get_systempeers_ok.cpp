#include "../src/polkadot.h"
#include "helpers/mockjsonrpc.h"
#undef NDEBUG
#include <cassert>

int main(int argc, char *argv[]) {
 
    auto app = polkadot::api::getInstance()->app();
    app->connect();

    auto resp3 = app->getSystemPeers();

    for(auto item : resp3->peers){
        assert(item.peerId > 0);
    }

    app->disconnect();

    cout << "success" << endl;

    return 0;
}
