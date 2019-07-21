
struct SystemInfo {
    char chainId[STRING_SIZE];
    char chainName[STRING_SIZE];
    char version[STRING_SIZE];
    int tokenDecimals;
    char tokenSymbol[STRING_SIZE];
};

struct SystemHealth{
    long peers;
    bool isSyncing;
    bool shouldHavePeers;
};