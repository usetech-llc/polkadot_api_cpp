#include "../src/polkadot.h"
#include "helpers/mockjsonrpc.h"
#undef NDEBUG
#include <cassert>

class CMockJsonRpcMissingField : public CMockJsonRpcMD0 {
protected:
    virtual Json getRuntimeVersion() {
        string err;
        return Json::parse("{\"apis\": [[\"0xdf6acb689907609b\", 2], [\"0x37e397fc7c91f5e4\", 1], "
                           "[\"0x40fe3ad401f8959a\", 3], [\"0xd2bc9897eed08f15\", 1], [\"0xf78b278be53f454c\", 1], "
                           "[\"0xaf2c0297a23e6d3d\", 1], [\"0xed99c5acb25eedf5\", 2], [\"0xdd718d5cc53262d4\", 1], "
                           "[\"0x7801759919ee83e5\", 1]], \"authoringVersion\": 1, \"implName\": \"parity-polkadot\", "
                           "\"implVersion\": 1, \"specName\": \"polkadot\"}",
                           err);
    }
};

int main(int argc, char *argv[]) {
    EasyLogger log;
    CMockJsonRpcMissingField mockRpc;

    CPolkaApi app(&log, &mockRpc);
    app.connect();

    unique_ptr<GetRuntimeVersionParams> par2(new GetRuntimeVersionParams);
    auto resp3 = app.getRuntimeVersion(move(par2));

    // Ensure all items are present in api
    int apiItemCount = 0;
    for (auto item : resp3->api) {
        if (item.id && strlen(item.num)) {
            apiItemCount++;
        }
    }
    assert(apiItemCount == 9);

    // Check authoringVersion
    assert(resp3->authoringVersion == 1);

    // Check implName
    assert(string(resp3->implName) == "parity-polkadot");

    // Check implVersion
    assert(resp3->implVersion == 1);

    // Check specName
    assert(string(resp3->specName) == "polkadot");

    // Check specVersion
    // API Method should not throw
    assert(resp3->specVersion == 0);

    app.disconnect();

    cout << "success" << endl;

    return 0;
}
