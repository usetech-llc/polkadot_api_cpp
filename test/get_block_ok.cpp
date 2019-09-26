#include "../src/polkadot.h"
#include "helpers/mockjsonrpc.h"
#undef NDEBUG
#include <cassert>

int main(int argc, char *argv[]) {

    auto app = polkadot::api::getInstance()->app();
    app->connect();

    cout << endl << endl << "============================ Get Block 1 Hash ============================" << endl;
    unique_ptr<GetBlockHashParams> parBH(new GetBlockHashParams);
    parBH->blockNumber = 1;
    auto respBHash = app->getBlockHash(move(parBH));

    cout << endl << endl << "============================ Get Block ============================" << endl;
    unique_ptr<GetBlockParams> par(new GetBlockParams);
    strcpy(par->blockHash, respBHash->hash);
    auto resp3 = app->getBlock(move(par));

    cout << endl << endl << "============================ Get Block Header ============================" << endl;
    unique_ptr<GetBlockParams> par2(new GetBlockParams);
    strcpy(par2->blockHash, respBHash->hash);
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
