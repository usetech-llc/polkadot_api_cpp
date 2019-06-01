#include "../src/polkadot.h"

int main(int argc, char *argv[])
{
    try {
        // Default constructor
        CWebSocketClient ws1();

        // URL parameter in constructor
        CWebSocketClient ws2(CConstants::parity_node_url);

        // All tests ran ok
        cout << "success" << endl;
    } catch (...) {
        cout << "failed" << endl;
    }

    return 0;
}
