#include "../src/polkadot.h"

int main(int argc, char *argv[]) {
    EasyLogger log;

    // Setup: remove certificate file
    rename(CConstants::certificate_file.c_str(), (CConstants::certificate_file + ".tmp").c_str());

    IWebSocketClient *ws = CWebSocketClient::getInstance(&log);
    int err = ws->connect(string("wss://alex.unfrastructure.io/public/ws"));
    if (err == 2) {
        cout << "success" << endl;
    }

    // Teardown: remove certificate file back
    rename((CConstants::certificate_file + ".tmp").c_str(), CConstants::certificate_file.c_str());

    delete ws;
    return 0;
}
