
struct TransferEra {
    char immortalEra[STRING_SIZE];
};

struct TransferMethod {
    char desc[STRING_SIZE];
    int value[STRING_SIZE];
};

struct Transfer {
    char version[STRING_SIZE];
    char signer[STRING_SIZE];
    char signature[STRING_SIZE];
    int nonce;
    TransferEra era;
    TransferMethod method;
};