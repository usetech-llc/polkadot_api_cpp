class CMockJsonRpc : public IJsonRpc {
private:
    virtual Json getRuntimeVersion() {
        string err;
        return Json::parse("{\"apis\": [[\"0xdf6acb689907609b\", 2], [\"0x37e397fc7c91f5e4\", 1], "
                           "[\"0x40fe3ad401f8959a\", 3], [\"0xd2bc9897eed08f15\", 1], [\"0xf78b278be53f454c\", 1], "
                           "[\"0xaf2c0297a23e6d3d\", 1], [\"0xed99c5acb25eedf5\", 2], [\"0xdd718d5cc53262d4\", 1], "
                           "[\"0x7801759919ee83e5\", 1]], \"authoringVersion\": 1, \"implName\": \"parity-polkadot\", "
                           "\"implVersion\": 1, \"specName\": \"polkadot\", \"specVersion\": 112}",
                           err);
    }

public:
    CMockJsonRpc() {}
    virtual ~CMockJsonRpc() override {}
    virtual int connect() { return 0; }
    virtual void disconnect() {}
    virtual Json request(Json jsonMap) {
        Json ret;

        if (jsonMap["method"] == "chain_getRuntimeVersion") {
            ret = getRuntimeVersion();
        }

        return move(ret);
    }
    int subscribeWs(Json jsonMap, IWebSocketMessageObserver *observer) { return 0; }
    int unsubscribeWs(int subscriptionId) { return 0; }
};
