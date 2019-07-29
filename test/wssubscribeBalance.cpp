#include "../src/polkadot.h"
#include "helpers/cli.h"
#undef NDEBUG
#include <cassert>

std::ostream &operator<<(std::ostream &dest, uint128 value) {
    std::ostream::sentry s(dest);
    if (s) {
        uint128 tmp = value < 0 ? -value : value;
        char buffer[128];
        char *d = std::end(buffer);
        do {
            --d;
            *d = "0123456789"[(int)(tmp % 10)];
            tmp /= 10;
        } while (tmp != 0);
        if (value < 0) {
            --d;
            *d = '-';
        }
        int len = std::end(buffer) - d;
        if (dest.rdbuf()->sputn(d, len) != len) {
            dest.setstate(std::ios_base::badbit);
        }
    }
    return dest;
}

int main(int argc, char *argv[]) {

    // Create and connect app layer
    JsonRpcParams params;
    params.jsonrpcVersion = "2.0";

    EasyLogger logger;
    CJsonRpc jsonRpc(CWebSocketClient::getInstance(&logger), &logger, params);

    CPolkaApi app(&logger, &jsonRpc);
    app.connect(getNodeUrlParam(argc, argv));

    // Subscribe to balance updates
    //string addr("5FpxCaAovn3t2sTsbBeT5pWTj2rg392E8QoduwAyENcPrKht");
    string addr("5HQdHxuPgQ1BpJasmm5ZzfSk5RDvYiH6YHfDJVE8jXmp4eig");
    bool done = false;
    uint128 balanceResult = (uint128)-1;
    app.subscribeBalance(addr, [&](uint128 balance) {
        cout << endl << "  Balance: " << (uint64_t)balance << endl << endl;
        balanceResult = balance;
        done = true;
    });

    // Wait until block number update arrives
    while (!done)
        usleep(10000);

    assert(balanceResult < (uint128)-1);

    // Uncomment if you want to watch for more updates
    // usleep(30000000);

    // Unsubscribe and close connection
    app.unsubscribeBalance(addr);
    app.disconnect();

    cout << "success" << endl;

    return 0;
}
