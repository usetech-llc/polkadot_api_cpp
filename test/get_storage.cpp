#include "../src/polkadot.h"
#include "helpers/mockjsonrpc.h"
#undef NDEBUG
#include <cassert>

int main(int argc, char *argv[]) {

    //string address = "5ECcjykmdAQK71qHBCkEWpWkoMJY6NXvpdKy8UeMx16q5gFr";
    string address = "5HQdHxuPgQ1BpJasmm5ZzfSk5RDvYiH6YHfDJVE8jXmp4eig";
    string module1 = "Balances";
    string variable1 = "FreeBalance";
    string module2 = "System";
    string variable2 = "AccountNonce";
    string module3 = "Timestamp";
    string variable3 = "Now";

    auto app = polkadot::api::getInstance()->app();
    app->connect();

    cout << endl << endl << "================== Get Storage 1: Address Balance ==================" << endl;
    Json prm1 = Json::object{{"type", "AccountId"}, {"value", address}};
    string balance = app->getStorage(prm1.dump(), module1, variable1);
    cout << endl << "Encoded balance: " << balance << endl;
    assert(balance != "null");
    assert(balance.length() > 0);

    cout << endl << endl << "================== Get Storage 2: Address Nonce ==================" << endl;
    string nonce = app->getStorage(prm1.dump(), module2, variable2);
    cout << endl << "Encoded nonce: " << nonce << endl;
    assert(nonce != "null");
    assert(nonce.length() > 0);

    cout << endl << endl << "================== Get Storage 3: Timestamp ==================" << endl;
    string timeNow = app->getStorage("", module3, variable3);
    cout << endl << "Encoded timestamp: " << timeNow << endl;
    assert(timeNow != "null");
    assert(timeNow.length() > 0);

    cout << endl << endl << "================== Get Storage 4: Non-existing storage key ==================" << endl;
    try {
        string abra = app->getStorage("", "Abra", "Cadabra");
        throw std::exception();
    } catch (ApplicationException) {
        cout << endl << "Caught expected ApplicationException" << endl << endl;
    }

    app->disconnect();

    cout << "success" << endl;

    return 0;
}
