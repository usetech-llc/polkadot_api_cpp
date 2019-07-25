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

    //{"id":3,"jsonrpc":"2.0","method":"author_submitAndWatchExtrinsic",
    //"params":["0x210281ffa673c814faabab0f81f2837d79df6aca044df12ba9b727110febf95bff2d0c01e214f265aee1c661e4ffaa109965
    //73172bd1b57b25020e78a787d7dfed89034ca52d40b2d22ecdc13607a74a675a985a15d490593337ec9a247c7789028d420204000300ff8eaf04
    //151687736326c9fea17e25fc5287613693c912909cb226aa4794f26a4804"]}

    // // sign message
    // char hw[] = "hello world";
    // auto msg = vector<uint8_t>(hw, hw + strlen(hw));
    // vector<uint8_t> sig(SR25519_SIGNATURE_SIZE, 0);
    // sr25519_sign(sig.data(), publicKey.data(), secretKey.data(), msg.data(), (size_t)msg.size());

    // cout << "================ signature " << endl;
    // for (int i = 0; i < SR25519_SIGNATURE_SIZE; ++i) {
    //     printf("%02X", sig[i]);
    // }
    // cout << endl;

    // // validate message with public key
    // assert(sr25519_verify(sig.data(), msg.data(), msg.size(), publicKey.data()));

    auto app = polkadot::api::getInstance()->app();
    app->connect();

Json js;

    app->submitAndSubcribeExtrinsic(NULL, js, NULL, NULL, NULL, [&](Json result) {
        cout << endl << " Account Nonce: " << result.dump() << endl << endl;
    });

    app->disconnect();


    cout << "success" << endl;
}
