#include "../src/polkadot.h"
#include "helpers/mockjsonrpc.h"
#undef NDEBUG
#include <cassert>

int main(int argc, char *argv[]) {
    auto app = polkadot::api::getInstance()->app();
    app->connect();

    auto pe = app->pendingExtrinsics();

    cout << endl << endl << "============================ Get Pending Extrinsics ============================" << endl;
    // auto pe = app->pendingExtrinsics();
    // cout << "Storage key for prefix \"" << module << " " << variable << "\" for address " << address << " : "
    //      << actualKey << endl;

    // // Check
    // assert(expectedKey == actualKey);
    // cout << "Matches expected key" << endl;

    app->disconnect();
    cout << "success" << endl;

    return 0;
}
