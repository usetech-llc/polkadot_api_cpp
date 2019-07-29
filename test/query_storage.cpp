#include "../src/polkadot.h"
#include "helpers/cli.h"
#undef NDEBUG
#include <cassert>
#include <set>

int main(int argc, char *argv[]) {

    auto app = polkadot::api::getInstance()->app();
    app->connect(getNodeUrlParam(argc, argv));

    // Get the most recent block number and hash
    auto lastblock = app->getBlock(nullptr);
    unique_ptr<GetBlockHashParams> prm(new GetBlockHashParams);
    long lastNumber = lastblock->block.header.number;
    prm->blockNumber = lastNumber;
    auto lastHash = app->getBlockHash(move(prm));

    // Get the 10 blocks back number and hash
    unique_ptr<GetBlockHashParams> prm2(new GetBlockHashParams);
    prm2->blockNumber = lastNumber - 10;
    auto tenBackHash = app->getBlockHash(move(prm2));

    cout << endl << "Most recent block    : " << lastNumber << ", hash: " << lastHash->hash << endl;
    cout << "10 blocks back block : " << lastNumber - 10 << ", hash: " << tenBackHash->hash << endl << endl;

    // Get timestamp history for last 50 blocks
    string module = "Timestamp";
    string variable = "Now";
    string key = app->getKeys("", module, variable);
    StorageItem itemBuf[20] = {0};
    int itemCount = app->queryStorage(key, tenBackHash->hash, lastHash->hash, itemBuf, 20);

    // Assert that item count is at least 10
    cout << endl << "Item count returned: " << itemCount << endl;
    assert(itemCount >= 10);
    cout << "Item count is OK" << endl << endl;

    // Assert that all items are different (time changes every block)
    set<string> values;
    for (int i = 0; i < itemCount; ++i) {
        cout << "Block: " << itemBuf[i].blockHash << ", value: " << itemBuf[i].value << endl;
        values.insert(itemBuf[i].value);
    }
    assert(values.size() == itemCount);
    cout << endl << "All timestamps are different" << endl << endl;

    app->disconnect();

    cout << "success" << endl;

    return 0;
}
