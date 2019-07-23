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

string StorageUtils::getMappedStorageKey(const Hasher hasher, const string &jsonPrm, const string &prefix) {

    // Parse parameters
    Json json;
    try {
        string err;
        json = Json::parse(jsonPrm, err);

        if (err.length() > 0) {
            throw ApplicationException(string("Invalid parameter string: ") + err);
        }
    } catch (ApplicationException) {
        throw;
    } catch (std::exception &e) {
        throw ApplicationException(string("Invalid parameter string was provided. Should be a JSON string and have two "
                                          "fields: type and value. Provided: ") +
                                   jsonPrm);
    }

    string type(json["type"].string_value());
    if (type == STORAGE_TYPE_ADDRESS) {
        Address addr;
        memcpy(addr.symbols, json["value"].string_value().c_str(), ADDRESS_LENGTH);
        return getAddressStorageKey(hasher, addr, prefix);
    } else if ((type == STORAGE_TYPE_BLOCK_NUMBER) || (type == STORAGE_TYPE_U32) ||
               (type == STORAGE_TYPE_ACCOUNT_INDEX) || (type == STORAGE_TYPE_PROPOSAL_INDEX) ||
               (type == STORAGE_TYPE_REFERENDUM_INDEX) || (type == STORAGE_TYPE_PARACHAIN_ID)) {
        char data[128] = {0};
        sprintf(data, "%s%s", prefix.c_str(), json["value"].string_value().c_str());
        return getStorageKey(hasher, (const unsigned char *)data, strlen(data));
    } else if (type == STORAGE_TYPE_HASH) {
        char data[256] = {0};
        sprintf(data, "%s%s", prefix.c_str(), json["value"].string_value().c_str());
        return getStorageKey(hasher, (const unsigned char *)data, strlen(data));
    }

    throw ApplicationException(string("Invalid parameter type: ") + json["type"].string_value());
}

string StorageUtils::getMappedStorageKey(const Hasher hasher, const char *jsonPrm, const char *prefix) {
    return getMappedStorageKey(hasher, string(jsonPrm), string(prefix));
}

string StorageUtils::getPlainStorageKey(const Hasher hasher, const string &prefix) {
    return getStorageKey(hasher, (const unsigned char *)prefix.c_str(), prefix.length());
}

string StorageUtils::getPlainStorageKey(const Hasher hasher, const char *prefix) {
    return getPlainStorageKey(hasher, string(prefix));
}
