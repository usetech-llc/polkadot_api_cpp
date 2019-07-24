#include "../src/polkadot.h"
#include "helpers/mockjsonrpc.h"
#undef NDEBUG
#include <cassert>

int main(int argc, char *argv[]) {

    auto app = polkadot::api::getInstance()->app();
    app->connect();

    string childKeysResponse = app->getChildKeys(string(""), string(""));
    cout << endl << "Child Keys Response: " << childKeysResponse << endl << endl;

    string childStorageResponse = app->getChildStorage(string(""), string(""));
    cout << endl << "Child Storage Response: " << childStorageResponse << endl << endl;

    string childStorageHashResponse = app->getChildStorageHash(string(""), string(""));
    cout << endl << "Child Storage Hash Response: " << childStorageHashResponse << endl << endl;

    int childStorageSizeResponse = app->getChildStorageSize(string(""), string(""));
    cout << endl << "Child Storage Size Response: " << childStorageSizeResponse << endl << endl;

    app->disconnect();

    cout << "success" << endl;

    return 0;
}
