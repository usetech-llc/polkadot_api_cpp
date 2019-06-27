#include "../src/polkadot.h"

int main(int argc, char *argv[]) {
    try {
        // Default constructor
        EasyLogger log;
        IWebSocketClient *ws = CWebSocketClient::getInstance(&log);

        // All tests ran ok
        cout << "success" << endl;

        delete ws;
    } catch (...) {
        cout << "failed" << endl;
    }

    return 0;
}
