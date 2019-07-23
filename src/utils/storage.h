#define STORAGE_KEY_BYTELENGTH 32

// Storage key types (as strings)
#define STORAGE_TYPE_ADDRESS "AccountId"
#define STORAGE_TYPE_BLOCK_NUMBER "BlockNumber"
#define STORAGE_TYPE_U32 "u32"
#define STORAGE_TYPE_ACCOUNT_INDEX "AccountIndex"
#define STORAGE_TYPE_PROPOSAL_INDEX "PropIndex"
#define STORAGE_TYPE_REFERENDUM_INDEX "ReferendumIndex"
#define STORAGE_TYPE_HASH "Hash"
#define STORAGE_TYPE_PARACHAIN_ID "ParaId"

class StorageUtils {
private:
public:
    static string getStorageKey(const Hasher hasher, const unsigned char *data, int length);

public:
    static string getAddressStorageKey(const Hasher hasher, const Address &address, const string &prefix);
    static string getAddressStorageKey(const Hasher hasher, const Address &address, const char *prefix);

    /**
     *  Generates storage key for a parameter with prefix. Parameter is a JSON string representing a value of certain
     * type, which has two fields: type and value. Type should be one of type strings defined above. Value should
     * correspond to the type. Example:
     *
     *    {"type" : "AccountId", "value" : "5ECcjykmdAQK71qHBCkEWpWkoMJY6NXvpdKy8UeMx16q5gFr"}
     *
     * @param hasher - hash function to use
     * @param jsonPrm - JSON string that contains parameter and its type
     * @param prefix - prefix including module and state variable separated by space (example: "Balances FreeBalance")
     */
    static string getMappedStorageKey(const Hasher hasher, const string &jsonPrm, const string &prefix);
    static string getMappedStorageKey(const Hasher hasher, const char *jsonPrm, const char *prefix);

    static string getPlainStorageKey(const Hasher hasher, const string &prefix);
    static string getPlainStorageKey(const Hasher hasher, const char *prefix);
};
