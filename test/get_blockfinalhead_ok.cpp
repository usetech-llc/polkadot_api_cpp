#include "../src/polkadot.h"
#include "helpers/mockjsonrpc.h"
#undef NDEBUG
#include <cassert>

int main(int argc, char *argv[]) {

    auto app = polkadot::api::getInstance()->app();
    app->connect();

    cout << endl << endl << "============================" << endl;
    auto resp5 = app->getFinalizedHead();

    // Check hash
    assert(strlen(resp5->blockHash) == 66);
    cout << endl << endl << "Finalized head hash: " << resp5->blockHash << endl << endl;

    app->disconnect();

    cout << "success" << endl;

    return 0;
}
