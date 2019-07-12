class AddressUtils {
private:
    static const char *const ALPHABET;
    static const char ALPHABET_MAP[128];
    static int EncodeBase58(const unsigned char *bytes, int len, unsigned char result[]);
    static int DecodeBase58(const unsigned char *str, int len, unsigned char *result);

public:
    static PublicKey getPublicKeyFromAddr(const Address &addr);
    static PublicKey getPublicKeyFromAddr(const string &addrStr);
    static PublicKey getPublicKeyFromAddr(const char *addrStr);
};
