#include "../src/polkadot.h"
#include "helpers/mockjsonrpc.h"
#undef NDEBUG
#include <cassert>

int main(int argc, char *argv[]) {

    string address = "5ECcjykmdAQK71qHBCkEWpWkoMJY6NXvpdKy8UeMx16q5gFr";
    string module = "Balances";
    string variable = "FreeBalance";

    auto app = polkadot::api::getInstance()->app();
    app->connect();

    string expectedKey = "0x01EF598903B784A4EB0E8BDCA123C55E";

    cout << endl << endl << "============================ Get Keys ============================" << endl;
    Json prm = Json::object{{"type", "AccountId"}, {"value", address}};
    string actualKey = app->getKeys(prm.dump(), module, variable);

    cout << "Storage key for prefix \"" << module << " " << variable << "\" for address " << address << " : "
         << actualKey << endl;

    // Check
    assert(expectedKey == actualKey);
    cout << "Matches expected key" << endl;

    app->disconnect();
    cout << "success" << endl;

    return 0;
}
