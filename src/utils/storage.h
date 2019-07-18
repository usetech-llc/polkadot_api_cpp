#define STORAGE_KEY_BYTELENGTH 32

class StorageUtils {
private:
public:
    static string getStorageKey(const Hasher hasher, const unsigned char *data, int length);

public:
    static string getAddressStorageKey(const Hasher hasher, const Address &address, const string &prefix);
    static string getAddressStorageKey(const Hasher hasher, const Address &address, const char *prefix);
};
