#include "../src/polkadot.h"
#include "helpers/mockjsonrpc.h"
#undef NDEBUG
#include <cassert>

int main(int argc, char *argv[]) {

    string address = "5ECcjykmdAQK71qHBCkEWpWkoMJY6NXvpdKy8UeMx16q5gFr";
    string module1 = "Balances";
    string variable1 = "FreeBalance";

    auto app = polkadot::api::getInstance()->app();
    app->connect();

    cout << endl << endl << "================== Get Storage Hash: Address Balance ==================" << endl;
    Json prm1 = Json::object{{"type", "AccountId"}, {"value", address}};
    string storageHash = app->getStorageHash(prm1.dump(), module1, variable1);
    cout << endl << "Storage hash: " << storageHash << endl << endl;

    app->disconnect();

    assert(storageHash.length() == 66);
    cout << "success" << endl;

    return 0;
}
