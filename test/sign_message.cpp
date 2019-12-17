// clang-format off
#include "../src/polkadot.h"
#undef NDEBUG
#include <cassert>
// clang-format on

using namespace std;

int main(int argc, char *argv[]) {
    // SR25519 keys
    vector<uint8_t> publicKey =
        fromHex<vector<uint8_t>>("5e8135dc17f025ca044780631ef89e21310b590b429e238786dd88dfec7b0f1d");
    vector<uint8_t> secretKey =
        fromHex<vector<uint8_t>>("60cda021deceff3c98d978537e584b433bffe0801337ff8b94df789cfed2ac5b27f5de3080641fded76ce"
                                 "cf59700813f03bdf3a3e5f77966ee66bea71f80f0a1");

    assert(publicKey.size() == PUBLIC_KEY_LENGTH);

    // sign message
    char hw[16] = "hello world";
    auto msg = vector<uint8_t>(hw, hw + strlen(hw));
    uint8_t sig[SR25519_SIGNATURE_SIZE + 16] = {0};
    //sr25519_sign(sig, publicKey.data(), secretKey.data(), msg.data(), (size_t)msg.size());
	//void sign011_s(uint8_t* public_key, uint8_t* secret_key, uint8_t* message, unsigned int message_size, uint8_t* result);

    // Sign using SR25519_C library
	sign011_s(publicKey.data(), secretKey.data(), msg.data(), strlen(hw), sig);

    cout << "Message: " << hw << endl;
    cout << "Signature: ";
    for (int i = 0; i < SR25519_SIGNATURE_SIZE; ++i) {
        printf("%02X", sig[i]);
    }
    cout << endl;

    // validate using sr25519_crust (Rust) library
    // assert(sr25519_verify(sig, msg.data(), msg.size(), publicKey.data()));
    // cout << "Signature verified successfully using Rust library" << endl;

    // validate using sr25519 C library
    bool verified = verify011_s(sig, publicKey.data(), msg.data(), msg.size());
    assert(verified);
    cout << "Signature verified successfully using C SR25519 library" << endl;

    // Negative test: validate with wrong message
    char hw_wrong[16] = "Goodbye world";
    auto msg_wrong = vector<uint8_t>(hw_wrong, hw_wrong + strlen(hw_wrong));
    bool verified_w = verify011_s(sig, publicKey.data(), msg_wrong.data(), msg_wrong.size());
    assert(!verified_w);
    cout << "Signature verification failed successfully with wrong message using C SR25519 library" << endl;


    cout << "success" << endl;
}
