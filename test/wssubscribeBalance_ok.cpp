#include "../src/polkadot.h"
#include "helpers/mockjsonrpc.h"
#undef NDEBUG
#include <cassert>

#define SUBSCRIPTION_ID 123
#define EXPECTED_BALANCE 445240000000000

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

class CMockJsonRpcBalanceSubscription : public CMockJsonRpcMD0 {
private:
    thread *_updateThread;
    IWebSocketMessageObserver *_observer;

    void updateGeneratorThread() {
        usleep(100000);

        string err;
        auto message =
            Json::parse("{\"block\":\"0xf26334cb2e870c90515ed8faa57a4d9712ec957e829518f9966d5bc15cf5a220\",\"changes\":"
                        "[[\"0x66f795b8d457430edda717c3cba459b9\",\"0x00300a84f19401000000000000000000\"]]}",
                        err);

        cout << err << endl;

        _observer->handleWsMessage(SUBSCRIPTION_ID, message);
    }

public:
    ~CMockJsonRpcBalanceSubscription() { _updateThread->join(); }
    virtual int subscribeWs(Json jsonMap, IWebSocketMessageObserver *observer) {
        _observer = observer;
        _updateThread = new thread(&CMockJsonRpcBalanceSubscription::updateGeneratorThread, this);
        return SUBSCRIPTION_ID;
    }
};

int main(int argc, char *argv[]) {
    EasyLogger log;
    CMockJsonRpcBalanceSubscription mockRpc;

    CPolkaApi app(&log, &mockRpc);
    app.connect();

    // Subscribe to balance updates
    string addr("5FpxCaAovn3t2sTsbBeT5pWTj2rg392E8QoduwAyENcPrKht");
    bool done = false;
    app.subscribeBalance(addr, [&](uint128 balance) {
        cout << endl << "  Balance: " << (uint64_t)balance << endl << endl;
        assert(EXPECTED_BALANCE == balance);
        done = true;
    });

    // Wait until block number update arrives
    while (!done)
        usleep(10000);

    // Unsubscribe and close connection
    app.unsubscribeBalance(addr);
    app.disconnect();

    cout << "success" << endl;

    return 0;
}
