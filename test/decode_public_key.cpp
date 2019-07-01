#include "../src/polkadot.h"

int main(int argc, char *argv[]) {

    Address addr;
    memcpy(addr.symbols, "5FpxCaAovn3t2sTsbBeT5pWTj2rg392E8QoduwAyENcPrKht", ADDRESS_LENGTH);

    decodePublicKeyFromAddr(addr);

    // 5FpxCaAovn3t2sTsbBeT5pWTj2rg392E8QoduwAyENcPrKht
    // 0x66f795b8d457430edda717c3cba459b9

    cout << "success" << endl;

    return 0;
}
