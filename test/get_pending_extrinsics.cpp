#include "../src/polkadot.h"
#include "helpers/mockjsonrpc.h"
#undef NDEBUG
#include <cassert>

int main(int argc, char *argv[]) {
    auto app = polkadot::api::getInstance()->app();
    app->connect();

    cout << endl << endl << "============================ Get Pending Extrinsics ============================" << endl;
    GenericExtrinsic peBuf[20];
    int count = app->pendingExtrinsics(peBuf, 20);

    cout << endl << count << " pending extrinsics returned" << endl;
    for (int i = 0; i < count; ++i) {
        if (i < 20) {
            // Checks
            assert(peBuf[i].signerAddress.length() != 0);
            assert(peBuf[i].length >= 100); // Minimum length == length of signature structure

            // Print
            cout << i << ": "
                 << "length = " << peBuf[i].length << ", version = " << (int)peBuf[i].signature.version
                 << ", signer pubkey = " << (int)peBuf[i].signature.signerPublicKey[0] << " "
                 << (int)peBuf[i].signature.signerPublicKey[1] << "..."
                 << ", module index = " << (int)peBuf[i].method.moduleIndex
                 << ", call index = " << (int)peBuf[i].method.methodIndex
                 << ", signer address = " << peBuf[i].signerAddress;
            cout << endl;
        }
    }

    app->disconnect();
    cout << "success" << endl;

    return 0;
}
