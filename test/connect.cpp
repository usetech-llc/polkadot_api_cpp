#include "../src/polkadot.h"

int main(int argc, char *argv[])
{
    CWebSocketClient ws(CConstants::parity_node_url);
    int err = ws.connect();
    if (err == 0)
        cout << "success" << endl;
    else
        cout << "Connection error: " << err << endl;

    return 0;
}
