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

    // Extract cli parameters
    string senderAddr(argv[1]);
    string recipientAddr(argv[2]);
    string amountStr(argv[3]);
    string senderPrivateKeyStr(argv[4]);

    // Connect
    auto app = polkadot::api::getInstance()->app();
    app->connect();

    // Send extrinsic without subscribing to updates
    auto receiverPublicKey = AddressUtils::getPublicKeyFromAddr(recipientAddr);
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

    string exHash =
        app->submitExtrinsic(buf2, mmWrittenLength, "balances", "transfer", senderAddr, senderPrivateKeyStr);

    cout << endl << "Sent extrinsic with hash: " << exHash << endl;
    cout << "Now let's try to cancel it... " << endl << endl;

    try {
        app->removeExtrinsic(exHash);
    } catch (ApplicationException) {
        cout << endl << "Yeah, looks like canceling is not yet supported" << endl << endl;
    }

    // Unsubscribe and close connection
    app->disconnect();

    cout << "success" << endl;

    return 0;
}
