#include "../src/polkadot.h"
#include "helpers/mockjsonrpc.h"
#undef NDEBUG
#include <cassert>

int main(int argc, char *argv[]) {
 
    auto app = polkadot::api::getInstance()->app();
    app->connect();

    auto resp5 = app->getFinalizedHead();

    // Check hash
    //assert(strcmp(resp5->blockHash, resp4->parentHash) == 0);

    cout << resp5->blockHash << endl;

    app->disconnect();

    cout << "success" << endl;

    return 0;
}
