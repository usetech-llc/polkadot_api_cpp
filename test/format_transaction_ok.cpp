#include "../src/polkadot.h"
#undef NDEBUG
#include <cassert>

int main(int argc, char *argv[]) {

    TransferExtrinsic te;
    memset(&te, 0, sizeof(te));
    te.method.moduleIndex = 3;
    te.method.methodIndex = 0;
    te.method.receiverPublicKey[0] = 0x01;
    te.method.receiverPublicKey[1] = 0x02;
    te.method.receiverPublicKey[2] = 0x03;
    te.method.amount = 1000000000000;
    te.signature.version = SIGNATURE_VERSION;
    te.signature.signerPublicKey[0] = 0x04;
    te.signature.signerPublicKey[1] = 0x05;
    te.signature.signerPublicKey[2] = 0x06;
    te.signature.sr25519Signature[0] = 0x11;
    te.signature.sr25519Signature[1] = 0x12;
    te.signature.sr25519Signature[2] = 0x13;
    te.signature.nonce = 3;
    te.signature.era = IMMORTAL_ERA;

    uint8_t buf[1024] = {0};
    long len = te.serializeBinary(buf);

    char ch[20];
    string printed;
    for (int i = 0; i < len; ++i) {
        sprintf(ch, "%02X", buf[i]);
        printed += ch;
    }
    // cout << printed << endl;

    string expected = "350281FF0405060000000000000000000000000000000000000000000000000000000000111213000000000000000000"
                      "000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000"
                      "000000000C000300FF0102030000000000000000000000000000000000000000000000000000000000070010A5D4E8";
    assert(expected == printed);

    cout << "success" << endl;

    return 0;
}
