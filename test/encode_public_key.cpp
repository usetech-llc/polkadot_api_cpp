#include "../src/polkadot.h"
#undef NDEBUG
#include <cassert>

int main(int argc, char *argv[]) {

    string expectedAddress("5FpxCaAovn3t2sTsbBeT5pWTj2rg392E8QoduwAyENcPrKht");
    auto keyBytes = fromHex<vector<uint8_t>>("a673c814faabab0f81f2837d79df6aca044df12ba9b727110febf95bff2d0c01", true);
    PublicKey pk;
    memcpy(pk.bytes, keyBytes.data(), SR25519_PUBLIC_SIZE);
    auto actualAddress = AddressUtils::getAddrFromPublicKey(pk);

    cout << actualAddress << endl;

    assert(expectedAddress == actualAddress);
    cout << "success" << endl;

    return 0;
}
