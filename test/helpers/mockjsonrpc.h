class CMockJsonRpc : public IJsonRpc {
protected:
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
    virtual int connect(string node_url = "") { return 0; }
    virtual void disconnect() {}
    virtual Json request(Json jsonMap) {
        Json ret;

        if (jsonMap["method"] == "chain_getRuntimeVersion") {
            ret = getRuntimeVersion();
        }

        return move(ret);
    }
    virtual int subscribeWs(Json jsonMap, IWebSocketMessageObserver *observer) { return 0; }
    virtual int unsubscribeWs(int subscriptionId) { return 0; }
};

class CMockJsonRpcMD0 : public CMockJsonRpc {
protected:
    virtual Json getMetadata() {
        string testValue;
        string line;
        ifstream myfile("test/metadata/exampleMetadataV0.txt");
        if (myfile.is_open()) {
            while (getline(myfile, line)) {
                testValue += line;
            }
            myfile.close();
        }

        string err;
        return Json("0x" + testValue);
    }

public:
    CMockJsonRpcMD0() {}
    virtual ~CMockJsonRpcMD0() override {}
    virtual Json request(Json jsonMap) {
        Json ret;

        if (jsonMap["method"] == "chain_getRuntimeVersion") {
            ret = getRuntimeVersion();
        }
        if (jsonMap["method"] == "state_getMetadata") {
            ret = getMetadata();
        }

        return move(ret);
    }
};

class CMockJsonRpcMD5 : public CMockJsonRpc {
protected:
    virtual Json getMetadata() {
        string testValue;
        string line;
        ifstream myfile("test/metadata/exampleMetadataV5.txt");
        if (myfile.is_open()) {
            while (getline(myfile, line)) {
                testValue += line;
            }
            myfile.close();
        }

        string err;
        return Json("0x" + testValue);
    }

public:
    CMockJsonRpcMD5() {}
    virtual ~CMockJsonRpcMD5() override {}
};

class CMockJsonRpcStateGetHashBlock : public CMockJsonRpcMD0 {
protected:
    virtual Json getBlockHash() {

        string err;
        return Json("0x37096ff58d1831c2ee64b026f8b70afab1942119c022d1dcfdbdc1558ebf63fa");
    }

public:
    CMockJsonRpcStateGetHashBlock() {}
    virtual ~CMockJsonRpcStateGetHashBlock() override {}
    virtual Json request(Json jsonMap) {
        Json ret;

        if (jsonMap["method"] == "chain_getBlockHash") {
            ret = getBlockHash();
            return move(ret);
        } else {
            return CMockJsonRpcMD0::request(jsonMap);
        }
    }
};

class CMockJsonRpcSystemInfo : public CMockJsonRpcMD0 {
protected:
    virtual Json getSystemProps() {

        string err;
        return Json::parse("\{\"tokenDecimals\":15,\"tokenSymbol\":\"DOT\"}", err);
    }

    virtual Json getSystemChain() { return Json("Alexander"); }

    virtual Json getSystemName() { return Json("parity-polkadot"); }

    virtual Json getSystemVersion() { return Json("0.4.4"); }

public:
    CMockJsonRpcSystemInfo() {}
    virtual ~CMockJsonRpcSystemInfo() override {}
    virtual Json request(Json jsonMap) {
        Json ret;

        if (jsonMap["method"] == "system_properties") {
            ret = getSystemProps();
        }

        else if (jsonMap["method"] == "system_chain") {
            ret = getSystemChain();
        }

        else if (jsonMap["method"] == "system_name") {
            ret = getSystemName();
        }

        else if (jsonMap["method"] == "system_version") {
            ret = getSystemVersion();
        }

        else {
            return CMockJsonRpcMD0::request(jsonMap);
        }

        return move(ret);
    }
};
