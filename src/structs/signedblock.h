enum DigestItemKey {
    Other,             // 0
    AuthoritiesChange, // 1
    ChangesTrieRoot,   // 2
    SealV0,            // 3
    Consensus,         // 4
    Seal,              // 5
    PreRuntime         // 6
};

struct MortalEra {
    unsigned long period;
    unsigned long phase;
};

// struct BlockExtrinsicEra{
//     char immortalEra[STRING_SIZE];
//     MortalEra mortalEra;
// };

// struct BlockExtrinsicSignature{
//     unsigned short int version;
//     char signer[STRING_SIZE]; // Address
//     char signature[STRING_SIZE];
//     int nonce;
//     BlockExtrinsicEra era;
// };

// struct BlockExtrinsicMethod{
//     unsigned short int callIndex;
//     unsigned short int args[STRING_SIZE];
// };

// struct BlockExtrinsic{
//     BlockExtrinsicMethod method;
//     BlockExtrinsicSignature signature;
// };

struct DigestItem {
    DigestItemKey key;
    char value[STRING_SIZE];
};

struct BlockHeader {
    char parentHash[STRING_SIZE];
    unsigned long long number;
    char stateRoot[STRING_SIZE];
    char extrinsicsRoot[STRING_SIZE];
    DigestItem digest[COLLECTION_SIZE];
};

struct Block {
    BlockHeader header;
    char extrinsic[COLLECTION_SIZE][STRING_SIZE];
};

struct SignedBlock {
    Block block;
    char justification[STRING_SIZE];
};

struct GetBlockParams {
    char blockHash[STRING_SIZE];
};

struct FinalHead {
    char blockHash[STRING_SIZE];
};
