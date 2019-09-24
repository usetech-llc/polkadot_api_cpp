#include "../src/polkadot.h"
#include "helpers/mockjsonrpc.h"
#undef NDEBUG
#include <cassert>

int main(int argc, char *argv[]) {

    auto app = polkadot::api::getInstance()->app();
    app->connect();

    unique_ptr<GetBlockHashParams> getBlockHashParams(new GetBlockHashParams);
    getBlockHashParams->blockNumber = 2;
    auto respGetBlockHash = app->getBlockHash(move(getBlockHashParams));

    cout << endl << endl << "============================ Get Block ============================" << endl;
    unique_ptr<GetBlockParams> par(new GetBlockParams);
    strcpy(par->blockHash, respGetBlockHash->hash);
    auto resp3 = app->getBlock(move(par));

    cout << endl << endl << "============================ Get Block Header ============================" << endl;
    unique_ptr<GetBlockParams> par2(new GetBlockParams);
    strcpy(par2->blockHash, respGetBlockHash->hash);
    auto resp4 = app->getBlockHeader(move(par2));

    cout << endl << endl << "Parent hash from block  : " << resp3->block.header.parentHash << endl;
    cout << "Parent hash from header : " << resp4->parentHash << endl;

    // Check hash
    assert(strcmp(resp3->block.header.parentHash, resp4->parentHash) == 0);
    assert(strlen(resp4->parentHash) == 66); // "64 bytes with 0x prefix"

    cout << "Received hashes match" << endl << endl;

    app->disconnect();

    cout << "success" << endl;

    return 0;
}
