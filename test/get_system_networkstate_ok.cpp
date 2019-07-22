#include "../src/polkadot.h"
#include "helpers/mockjsonrpc.h"
#undef NDEBUG
#include <cassert>

int main(int argc, char *argv[]) {

    auto app = polkadot::api::getInstance()->app();
    app->connect();

    cout << endl << endl << "============================ Get Network State ============================" << endl;
    auto resp3 = app->getNetworkState();

    cout << endl << endl << "Peer Id                     : " << resp3->peerId << endl;
    cout << "Average Download Per Second : " << resp3->AverageDownloadPerSec << endl;
    cout << "Average Upload Per Second   : " << resp3->AverageUploadPerSec << endl << endl;

    assert(strcmp(resp3->connectedPeers->key, "") != 0);
    assert(strcmp(resp3->peerId, "") != 0);

    app->disconnect();

    cout << "success" << endl;

    return 0;
}
