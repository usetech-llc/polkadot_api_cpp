
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
    int count;
    PeerInfo peers[MAX_PEER_COUNT];
};

struct Endpoint {
    char dialing[STRING_SIZE];
};

struct ConnectedPeerTime {
    unsigned long nanos;
    unsigned long secs;
};

struct ConnectedPeerInfo {
    bool enabled;
    Endpoint endpoint;
    char knownAddresses[COLLECTION_SIZE][STRING_SIZE];
    ConnectedPeerTime latestPingTime;
    bool open;
    char versionString[STRING_SIZE];
};

struct NotConnectedPeerInfo {
    char knownAddresses[COLLECTION_SIZE][STRING_SIZE];
};

struct ConnectedPeer {
    char key[STRING_SIZE];
    ConnectedPeerInfo connectedPeerInfo;
};

struct NotConnectedPeer {
    char key[STRING_SIZE];
    NotConnectedPeerInfo notConnectedPeerInfo;
};

struct NetworkState {
    unsigned int AverageDownloadPerSec;
    unsigned int AverageUploadPerSec;
    ConnectedPeer connectedPeers[COLLECTION_SIZE];
    char externalAddresses[COLLECTION_SIZE][STRING_SIZE];
    char listenedAddresses[COLLECTION_SIZE][STRING_SIZE];
    NotConnectedPeer notConnectedPeers[COLLECTION_SIZE];
    char peerId[STRING_SIZE];
    char peerset[STRING_SIZE];
};