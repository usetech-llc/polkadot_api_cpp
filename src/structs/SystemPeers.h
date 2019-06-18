/*
  response system_peers
*/
enum ROLES { AUTHORITY, FULL };

struct PeerItem {
    char peerId[MAX_NAME_SIZE];
    ROLES roles;
    int protocolVersion;
    char bestHash[MAX_NAME_SIZE];
    int bestNumber;
};

struct SystemPeers {
    PeerItem peers[DEFAULT_SIZE];
};
