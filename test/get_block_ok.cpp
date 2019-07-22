#include "../src/polkadot.h"
#include "helpers/mockjsonrpc.h"
#undef NDEBUG
#include <cassert>

int main(int argc, char *argv[]) {

    auto app = polkadot::api::getInstance()->app();
    app->connect();

    unique_ptr<GetBlockParams> par(new GetBlockParams);
    strcpy(par->blockHash, "0x37096ff58d1831c2ee64b026f8b70afab1942119c022d1dcfdbdc1558ebf63fa");
    auto resp3 = app->getBlock(move(par));

    unique_ptr<GetBlockParams> par2(new GetBlockParams);
    strcpy(par2->blockHash, "0x37096ff58d1831c2ee64b026f8b70afab1942119c022d1dcfdbdc1558ebf63fa");
    auto resp4 = app->getBlockHeader(move(par2));

    // Check hash
    assert(strcmp(resp3->block.header.parentHash, resp4->parentHash) == 0);
    assert(strlen(resp4->parentHash) == 66); // "64 bytes with 0x prefix"

    app->disconnect();

    cout << "success" << endl;

    return 0;
}
