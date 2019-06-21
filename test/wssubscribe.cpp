#include "../src/polkadot.h"

class TestSubscriptionListener : public IWebSocketMessageObserver {
public:
    volatile bool done;
    LoggerStub log;
    IWebSocketClient *client;
    CJsonRpc *jr;
    int subscriptionId;
    thread *disconnectThread;

    TestSubscriptionListener() : done(false), disconnectThread(nullptr) {
        JsonRpcParams params;
        params.jsonrpcVersion = "2.0";

        // Connect
        client = CWebSocketClient::getInstance(&log);
        client->connect();
        jr = new CJsonRpc(client, &log, params);
    }

    ~TestSubscriptionListener() {
        delete jr;
        disconnectThread->join();
        delete disconnectThread;
        delete client;
    }

    void subscribe() {

        // Subscription request for timestamp
        auto timestampCmd = Json::array{"0x0e4944cfd98d6f4cc374d16f5a4e3f9c"};
        Json subscribeQuery = Json::object{{"method", "state_subscribeStorage"}, {"params", Json::array{timestampCmd}}};

        // Subscribe
        subscriptionId = jr->subscribeWs(subscribeQuery, this);
    }

    virtual void handleWsMessage(const Json &message) {
        cout << "WS Update Message Received (console app): " << message.dump() << endl;
        if (!disconnectThread)
            disconnectThread = new thread(&TestSubscriptionListener::close, this);
    }

    void close() {
        // Unsubscribe
        jr->unsubscribeWs(subscriptionId);

        // Disconnect
        CWebSocketClient::getInstance(nullptr)->disconnect();
        done = true;
    }
};

int main(int argc, char *argv[]) {

    TestSubscriptionListener tcl;
    tcl.subscribe();

    while (!tcl.done)
        usleep(10000);

    cout << "success" << endl;

    return 0;
}
