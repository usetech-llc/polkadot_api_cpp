#include "../src/polkadot.h"

class MetadataUtils {
public:
    static unsigned int GetModuleIndex(string moduleName, Metadata metadata) { return 0; };
};

int main(int argc, char *argv[]) {

    // This is a manual test due to lack of test DOTs
    if (argc < 4) {
        cout << "This is intended to be a manual test" << endl;
        cout << "Usage: ";
        cout << argv[0] << " <sender address> <recipient address> <amount in fDOTs> <sender private key (hex)>" << endl;
        cout << "success" << endl;
        return 0;
    }

    // Extract parameters
    string senderAddr(argv[1]);
    string recipientAddr(argv[2]);
    string amountStr(argv[3]);
    string senderPrivateKeyStr(argv[4]);

    JsonRpcParams params;
    params.jsonrpcVersion = "2.0";

    EasyLogger logger;
    CJsonRpc jsonRpc(CWebSocketClient::getInstance(&logger), &logger, params);

    CPolkaApi app(&logger, &jsonRpc);
    app.connect();

    // Subscribe to account nonce updates
    bool done = false;

    auto receiverPublicKey = AddressUtils::getPublicKeyFromAddr(recipientAddr);
    // auto receiverPublicKey = AddressUtils::getPublicKeyFromAddr("5HQdHxuPgQ1BpJasmm5ZzfSk5RDvYiH6YHfDJVE8jXmp4eig");

    uint8_t receiverBytes[SR25519_PUBLIC_SIZE];
    memcpy(receiverBytes, receiverPublicKey.bytes, SR25519_PUBLIC_SIZE);

    int mmWrittenLength = 0;
    u_int8_t buf2[2048];

    // Receiving address public key
    memcpy(buf2 + mmWrittenLength, receiverBytes, SR25519_PUBLIC_SIZE);
    mmWrittenLength += SR25519_PUBLIC_SIZE;

    // Compact-encode amount
    auto compactAmount = scale::encodeCompactInteger(stoll(amountStr));

    // Amount
    mmWrittenLength += scale::writeCompactToBuf(compactAmount, buf2 + mmWrittenLength);

    app.submitAndSubcribeExtrinsic(buf2, mmWrittenLength, "balances", "transfer", senderAddr, senderPrivateKeyStr,
                                   [&](Json response) {
                                       cout << endl << endl << "Response json:  " << response.dump() << endl;
                                       done = true;
                                   });

    // example
    // app.submitAndSubcribeExtrinsic(buf2, mmWrittenLength, "balances", "transfer",
    // "5GuuxfuxbvaiwteUrV9U7Mj2Fz7TWK84WhLaZdMMJRvSuzr4",
    // "0xa81056d713af1ff17b599e60d287952e89301b5208324a0529b62dc7369c745defc9c8dd67b7c59b201bc164163a8978d40010c22743db142a47f2e064480d4b",
    // [&](Json response){
    //     cout << endl << endl << "Response json:  " << response.dump() << endl;
    //     done = true;
    // });

    // Wait until transaction is mined
    while (!done)
        usleep(10000);

    // Unsubscribe and close connection
    app.disconnect();

    cout << "success" << endl;

    return 0;
}
