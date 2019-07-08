#include "../src/polkadot.h"
#undef NDEBUG
#include <cassert>

int main(int argc, char *argv[]) {

    Address addr;
    memcpy(addr.symbols, "5FpxCaAovn3t2sTsbBeT5pWTj2rg392E8QoduwAyENcPrKht", ADDRESS_LENGTH);

    string expectedStorageKey("0x216F908CA0FC8CD350701926F1005F6D7D512B20354458E2D2170F85A278F909");
    string actualStorageKey = StorageUtils::getAddressStorageKey(BLAKE2, addr, "Balances FreeBalance");

    assert(expectedStorageKey == actualStorageKey);
    assert(expectedStorageKey.length() == 32 * 2 + 2);
    cout << "success" << endl;

    return 0;
}
