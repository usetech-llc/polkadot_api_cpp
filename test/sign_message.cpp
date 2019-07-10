// clang-format off
#include <algorithm>
#include <iostream>
#include <random>
#include <vector>
#include "../src/polkadot.h"
#undef NDEBUG
#include <cassert>
// clang-format on

using namespace std;

inline std::vector<uint8_t> operator"" _fromChars(const char *c, size_t s) { return std::vector<uint8_t>{c, c + s}; }

inline std::vector<uint8_t> operator"" _fromHex(const char *c, size_t s) {
    std::string hex{c, c + s};
    std::vector<uint8_t> v;

    int len = hex.length();
    std::string newString;
    for (auto i = 0; i < len; i += 2) {
        std::string byte = hex.substr(i, 2);
        char chr = (char)strtol(byte.c_str(), nullptr, 16);
        v.push_back(chr);
    }

    return v;
}

int main(int argc, char *argv[]) {
    // SR25519 keys
    std::vector<uint8_t> publicKey = "5e8135dc17f025ca044780631ef89e21310b590b429e238786dd88dfec7b0f1d"_fromHex;
    std::vector<uint8_t> secretKey =
        "60cda021deceff3c98d978537e584b433bffe0801337ff8b94df789cfed2ac5b27f5de3080641fded76cecf59700813f03bdf3a3e5f77966ee66bea71f80f0a1"_fromHex;

    assert(publicKey.size() == PUBLIC_KEY_LENGTH);

    // sign message
    auto msg = "hello world"_fromChars;
    std::vector<uint8_t> sig(SR25519_SIGNATURE_SIZE, 0);
    sr25519_sign(sig.data(), publicKey.data(), secretKey.data(), msg.data(), (size_t)msg.size());

    // validate message with public key
    assert(sr25519_verify(sig.data(), msg.data(), msg.size(), publicKey.data()));

    cout << "success" << endl;
}
