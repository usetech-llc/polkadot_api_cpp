#include "../src/polkadot.h"

class TestMessageHandler : public IMessageObserver {
public:
    virtual void handleMessage(const string &payload) {
        Json json;
        string err;
        json = Json::parse(payload, err);

        cout << endl << "JSON RPC Version: " << json["jsonrpc"].string_value() << endl;
        cout << "Request ID: " << json["id"].int_value() << endl;
        cout << "Spec Name: " << json["result"]["specName"].string_value() << endl;
    }
};

int main(int argc, char *argv[]) {
    TestMessageHandler tmh;

    IWebSocketClient *ws = CWebSocketClient::getInstance();
    ws->registerMessageObserver(&tmh);
    int err = ws->connect();
    usleep(2000000);
    if ((err == 0) && (ws->isConnected())) {
        std::string msg = "{\"id\":2,\"jsonrpc\":\"2.0\",\"method\":\"chain_getRuntimeVersion\",\"params\":[]}";
        ws->send(msg);

        usleep(2000000);
        ws->disconnect();
        cout << "success" << endl;
    } else {
        cout << "Connection error: " << err << endl;
    }

    usleep(500000);

    delete ws;
    return 0;
}
