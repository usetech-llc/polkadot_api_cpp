#include "../src/polkadot.h"
#include "helpers/mockjsonrpc.h"
#undef NDEBUG
#include <cassert>

int main(int argc, char *argv[]) {

    string address = "5ECcjykmdAQK71qHBCkEWpWkoMJY6NXvpdKy8UeMx16q5gFr";
    string module1 = "Balances";
    string variable1 = "FreeBalance";
    int expectedBalanceSize = 16; // 128 bits

    auto app = polkadot::api::getInstance()->app();
    app->connect();

    cout << endl << endl << "================== Get Storage Size: Address Balance ==================" << endl;
    Json prm1 = Json::object{{"type", "AccountId"}, {"value", address}};
    int storageSize = app->getStorageSize(prm1.dump(), module1, variable1);
    cout << endl << "Storage size: " << storageSize << endl << endl;

    app->disconnect();

    assert(storageSize == expectedBalanceSize);
    cout << "success" << endl;

    return 0;
}
