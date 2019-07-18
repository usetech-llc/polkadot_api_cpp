#include "../../src/polkadot.h"
#include "../libs/blake/blake2.h"
#include "../libs/xxhash/xxhash.h"

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
