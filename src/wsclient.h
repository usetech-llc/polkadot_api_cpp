class CWebSocketClient {
private:
    string _nodeUrl;

public:
    CWebSocketClient();
    CWebSocketClient(string nodeUrl);

    int connect();
    bool isConnected();
    void disconnect();

};
