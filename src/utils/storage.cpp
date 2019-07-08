#include "../../src/polkadot.h"
#include "../libs/blake/blake2.h"
#include "../libs/xxhash/xxhash.h"

/*
string StorageUtils::getStorageKey(Hasher hasher, const unsigned char *data, int length) {
    PublicKey pubk{0};

    unsigned char bs58decoded[ADDRESS_LENGTH];
    int len = DecodeBase58(addr.symbols, ADDRESS_LENGTH, bs58decoded);
    cout << len << endl;

    memcpy(pubk.bytes, bs58decoded + 1, PUBLIC_KEY_LENGTH);
    // a673c814faabab0f81f2837d79df6aca044df12ba9b727110febf95bff2d0c01
    printf("%02X %02X %02X %02X\n", pubk.bytes[0], pubk.bytes[1], pubk.bytes[2], pubk.bytes[3]);

    char pref[] = "Balances FreeBalance";
    unsigned char tmp[200] = {0};
    memcpy(tmp, pref, strlen(pref));
    memcpy(tmp + strlen(pref), pubk.bytes, 32);

    unsigned char blake2bHashed[100];
    blake2b(blake2bHashed, 32, tmp, strlen(pref) + 32, NULL, 0);
    printf("0x");
    for (int i = 0; i < 32; i++)
        printf("%02X", blake2bHashed[i]);
    printf("\n");

    auto xxhash = XXH64(tmp, strlen(pref) + 32, 0);
    unsigned char bytes1[8] = {(unsigned char)(xxhash & 0xFF),
                               (unsigned char)(xxhash & 0xFF00) >> 8,
                               (unsigned char)(xxhash & 0xFF0000) >> 16,
                               (unsigned char)(xxhash & 0xFF000000) >> 24,
                               (unsigned char)(xxhash & 0xFF00000000) >> 32,
                               (unsigned char)(xxhash & 0xFF0000000000) >> 40,
                               (unsigned char)(xxhash & 0xFF000000000000) >> 48,
                               (unsigned char)(xxhash & 0xFF00000000000000) >> 56};
    printf("0x");
    for (int i = 0; i < 8; i++)
        printf("%02X", bytes1[i]);
    printf("\n");

    auto xxhash2 = XXH64(tmp, strlen(pref) + 32, 1);
    unsigned int h2 = (xxhash2 & 0xFFFFFFFF00000000) >> 32;
    unsigned int l2 = xxhash2 & 0xFFFFFFFF;
    printf("0x%X %X", h2, l2);
    printf("\n");

    return move(pubk);
}
*/

string StorageUtils::getStorageKey(const Hasher hasher, const unsigned char *data, int length) {
    char key[2 * STORAGE_KEY_BYTELENGTH + 3] = {0};

    if (hasher == XXHASH) {
        auto xxhash1 = XXH64(data, length, 0);
        unsigned char bytes1[8] = {(unsigned char)(xxhash1 & 0xFF),
                                   (unsigned char)((xxhash1 & 0xFF00) >> 8),
                                   (unsigned char)((xxhash1 & 0xFF0000) >> 16),
                                   (unsigned char)((xxhash1 & 0xFF000000) >> 24),
                                   (unsigned char)((xxhash1 & 0xFF00000000) >> 32),
                                   (unsigned char)((xxhash1 & 0xFF0000000000) >> 40),
                                   (unsigned char)((xxhash1 & 0xFF000000000000) >> 48),
                                   (unsigned char)((xxhash1 & 0xFF00000000000000) >> 56)};

        auto xxhash2 = XXH64(data, length, 1);
        unsigned char bytes2[8] = {(unsigned char)(xxhash2 & 0xFF),
                                   (unsigned char)((xxhash2 & 0xFF00) >> 8),
                                   (unsigned char)((xxhash2 & 0xFF0000) >> 16),
                                   (unsigned char)((xxhash2 & 0xFF000000) >> 24),
                                   (unsigned char)((xxhash2 & 0xFF00000000) >> 32),
                                   (unsigned char)((xxhash2 & 0xFF0000000000) >> 40),
                                   (unsigned char)((xxhash2 & 0xFF000000000000) >> 48),
                                   (unsigned char)((xxhash2 & 0xFF00000000000000) >> 56)};

        sprintf(key, "0x");
        for (int i = 0; i < 8; i++)
            sprintf(key + strlen(key), "%02X", bytes1[i]);
        for (int i = 0; i < 8; i++)
            sprintf(key + strlen(key), "%02X", bytes2[i]);

    } else if (hasher == BLAKE2) {
        unsigned char blake2bHashed[32];
        blake2b(blake2bHashed, 32, data, length, NULL, 0);

        sprintf(key, "0x");
        for (int i = 0; i < 32; i++)
            sprintf(key + strlen(key), "%02X", blake2bHashed[i]);
    }

    return move(string(move(key)));
}

string StorageUtils::getAddressStorageKey(const Hasher hasher, const Address &address, const string &prefix) {
    PublicKey pubk = AddressUtils::getPublicKeyFromAddr(address);

    unsigned char data[PUBLIC_KEY_LENGTH + STRING_SIZE] = {0};
    memcpy(data, prefix.c_str(), prefix.length());
    memcpy(data + prefix.length(), pubk.bytes, PUBLIC_KEY_LENGTH);

    return getStorageKey(hasher, data, PUBLIC_KEY_LENGTH + prefix.length());
}

string StorageUtils::getAddressStorageKey(const Hasher hasher, const Address &address, const char *prefix) {
    return getAddressStorageKey(hasher, address, string(prefix));
}
