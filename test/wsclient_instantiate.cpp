#include "../src/polkadot.h"

int main(int argc, char *argv[]) {
    try {
        // Default constructor
        CWebSocketClient *ws = CWebSocketClient::getInstance();

        // All tests ran ok
        cout << "success" << endl;

        delete ws;
    } catch (...) {
        cout << "failed" << endl;
    }

    return 0;
}
