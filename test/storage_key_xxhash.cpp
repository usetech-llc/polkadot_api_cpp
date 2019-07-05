#include "../src/polkadot.h"
#undef NDEBUG
#include <cassert>

int main(int argc, char *argv[]) {

    Address addr;
    memcpy(addr.symbols, "5FpxCaAovn3t2sTsbBeT5pWTj2rg392E8QoduwAyENcPrKht", ADDRESS_LENGTH);

    string expectedStorageKey("0x66F795B8D457430EDDA717C3CBA459B9");
    string actualStorageKey = StorageUtils::getAddressStorageKey(XXHASH, addr, "Balances FreeBalance");

    assert(expectedStorageKey == actualStorageKey);
    assert(expectedStorageKey.length() == 16 * 2 + 2);
    cout << "success" << endl;

    return 0;
}
