#include "../src/polkadot.h"
#include "helpers/mockjsonrpc.h"
#undef NDEBUG
#include <cassert>

int main(int argc, char *argv[]) {
    EasyLogger log;
    CMockJsonRpcStateGetHashBlock mockRpc;

    CPolkaApi app(&log, &mockRpc);
    app.connect();

    unique_ptr<GetBlockHashParams> par(new GetBlockHashParams);
    par->blockNumber = 2;
    auto resp3 = app.getBlockHash(move(par));

    // Check hash
    assert(strcmp(resp3->hash, "0x37096ff58d1831c2ee64b026f8b70afab1942119c022d1dcfdbdc1558ebf63fa") == 0);

    app.disconnect();

    cout << "success" << endl;

    return 0;
}
