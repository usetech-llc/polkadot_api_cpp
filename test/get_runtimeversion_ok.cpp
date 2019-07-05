#include "../src/polkadot.h"
#include "helpers/mockjsonrpc.h"
#undef NDEBUG
#include <cassert>

int main(int argc, char *argv[]) {
    EasyLogger log;
    CMockJsonRpcMD0 mockRpc;

    CPolkaApi app(&log, &mockRpc);
    app.connect();

    unique_ptr<GetRuntimeVersionParams> par2(new GetRuntimeVersionParams);
    auto resp3 = app.getRuntimeVersion(move(par2));

    // Ensure all items are present in api
    int apiItemCount = 0;
    for (auto item : resp3->api) {
        if (item.id && strlen(item.num))
            apiItemCount++;
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
    assert(resp3->specVersion == 112);

    app.disconnect();

    cout << "success" << endl;

    return 0;
}
