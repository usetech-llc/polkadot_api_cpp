#include "../src/polkadot.h"
#undef NDEBUG
#include <cassert>

int main(int argc, char *argv[]) {

    string expectedPublicKeyStr("a673c814faabab0f81f2837d79df6aca044df12ba9b727110febf95bff2d0c01");
    auto actualPublicKey = AddressUtils::getPublicKeyFromAddr("5FpxCaAovn3t2sTsbBeT5pWTj2rg392E8QoduwAyENcPrKht");

    char actualPublicKeyCh[PUBLIC_KEY_LENGTH * 2 + 3] = {0};
    for (int i = 0; i < PUBLIC_KEY_LENGTH; ++i) {
        sprintf(actualPublicKeyCh + strlen(actualPublicKeyCh), "%02x", actualPublicKey.bytes[i]);
    }
    string actualPublicKeyStr(actualPublicKeyCh);
    assert(expectedPublicKeyStr == actualPublicKeyStr);
    cout << "success" << endl;

    return 0;
}
