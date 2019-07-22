
struct SystemInfo {
    char chainId[STRING_SIZE];
    char chainName[STRING_SIZE];
    char version[STRING_SIZE];
    int tokenDecimals;
    char tokenSymbol[STRING_SIZE];
};

struct SystemHealth {
    long peers;
    bool isSyncing;
    bool shouldHavePeers;
};

struct PeerInfo {
    char bestHash[STRING_SIZE];
    unsigned long bestNumber;
    char peerId[STRING_SIZE];
    unsigned int protocolVersion;
    char roles[STRING_SIZE];
};

struct PeersInfo {
    PeerInfo peers[COLLECTION_SIZE * STRING_SIZE];
};