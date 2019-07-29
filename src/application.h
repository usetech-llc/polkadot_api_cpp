// lastLengthChange storage subscription hash
#define LAST_LENGTH_CHANGE_SUBSCRIPTION "0xe781aa1e06ea53e01a4e129e0496764e"
// sessionLenth storage subscription hash
#define SESSION_LENGTH_SUBSCRIPTION "0xd9c94b41dc87728ebf0a966d2e9ad9c0"
// currentEra storage subscription hash
#define CURRENT_ERA_SUBSCRIPTION "0x579ab55d37b1220812be3c3df29d4858"
// sessionsPerEra storage subscription hash
#define SESSIONS_PER_ERA_SUBSCRIPTION "0xb7b6ec0f25eb1ed8b91d05f697d7a874"
// currentIndex storage subscription hash
#define CURRENT_INDEX_SUBSCRIPTION "0xb8f48a8c01f629d6dc877f64892bed49"

#define MAX_METHOD_BYTES_SZ 2048

typedef struct ProtocolParameters {
    Hasher FreeBalanceHasher;
    string FreeBalancePrefix;
    int BalanceModuleIndex;
    int TransferMethodIndex;
    uint8_t GenesisBlockHash[BLOCK_HASH_SIZE];
    unique_ptr<Metadata> metadata;
} ProtocolParameters;

typedef struct SubscriptionUpdate {
    int subscriptionId;
    Json message;
} SubscriptionUpdate;

class ApplicationException : public exception {
    string _msg;

public:
    ApplicationException(string msg) { _msg = msg; };
    virtual char const *what() const noexcept { return _msg.c_str(); }
};

class CPolkaApi : public IApplication, IWebSocketMessageObserver {
private:
    ILogger *_logger;
    IJsonRpc *_jsonRpc;

    unique_ptr<SystemInfo> createSystemInfo(Json jsonObject);
    unique_ptr<BlockHash> createBlockHash(Json jsonObject);
    unique_ptr<RuntimeVersion> createRuntimeVersion(Json jsonObject);
    unique_ptr<Metadata> createMetadata(Json jsonObject);
    unique_ptr<SignedBlock> createBlock(Json jsonObject);
    unique_ptr<BlockHeader> createBlockHeader(Json jsonObject);
    unique_ptr<FinalHead> createFinalHead(Json jsonObject);
    unique_ptr<SystemHealth> createSystemHealth(Json jsonObject);
    unique_ptr<PeersInfo> createPeerInfo(Json jsonObject);
    unique_ptr<NetworkState> createNetworkState(Json jsonObject);
    template <typename T, unique_ptr<T> (CPolkaApi::*F)(Json)> unique_ptr<T> deserialize(Json jsonObject);
    Hasher getFuncHasher(unique_ptr<Metadata> &meta, const string &moduleName, const string &funcName);
    int getModuleIndex(unique_ptr<Metadata> &meta, const string &moduleName, bool skipZeroCalls);
    int getCallMethodIndex(unique_ptr<Metadata> &meta, const int moduleIndex, const string &funcName);
    int getStorageMethodIndex(unique_ptr<Metadata> &meta, const int moduleIndex, const string &funcName);
    bool hasMethods(unique_ptr<Metadata> &meta, const int moduleIndex);
    bool isStateVariablePlain(unique_ptr<Metadata> &meta, const int moduleIndex, const int varIndex);

    ProtocolParameters _protocolPrm;
    long long _bestBlockNum;

    // Implements IWebSocketMessageObserver
    void handleWsMessage(const int subscriptionId, const Json &message);

    // Subscriber functors
    std::function<void(long long)> _blockNumberSubscriber;
    map<string, std::function<void(uint128)>> _balanceSubscribers;
    map<string, std::function<void(unsigned long)>> _nonceSubscribers;
    std::function<void(Era, Session)> _eraAndSessionSubscriber;
    std::function<void(string)> _transactionCompletionSubscriber;

    // Subscription IDs
    int _blockNumberSubscriptionId;
    map<string, int> _balanceSubscriptionIds;
    map<string, int> _nonceSubscriptionIds;
    int _eraAndSessionSubscriptionId;
    int _transactionCompletionSubscriptionId;

public:
    CPolkaApi() = delete;
    CPolkaApi(ILogger *logger, IJsonRpc *jsonRpc);
    virtual ~CPolkaApi() override {}
    virtual int connect(string node_url = "");
    virtual void disconnect();
    virtual unique_ptr<SystemInfo> getSystemInfo();
    virtual unique_ptr<BlockHash> getBlockHash(unique_ptr<GetBlockHashParams> params);
    virtual unique_ptr<Metadata> getMetadata(unique_ptr<GetMetadataParams> params);
    virtual unique_ptr<RuntimeVersion> getRuntimeVersion(unique_ptr<GetRuntimeVersionParams> params);
    virtual unique_ptr<SignedBlock> getBlock(unique_ptr<GetBlockParams> params);
    virtual unique_ptr<BlockHeader> getBlockHeader(unique_ptr<GetBlockParams> params);
    virtual unique_ptr<FinalHead> getFinalizedHead();
    virtual unique_ptr<SystemHealth> getSystemHealth();
    virtual unique_ptr<PeersInfo> getSystemPeers();
    virtual unique_ptr<NetworkState> getNetworkState();
    virtual unsigned long getAccountNonce(string address);
    virtual string getKeys(const string &jsonPrm, const string &module, const string &variable);
    virtual string getStorage(const string &jsonPrm, const string &module, const string &variable);
    virtual string getStorageHash(const string &jsonPrm, const string &module, const string &variable);
    virtual int getStorageSize(const string &jsonPrm, const string &module, const string &variable);

    virtual void signAndSendTransfer(string sender, string privateKey, string recipient, uint128 amount,
                                     std::function<void(string)> callback);
    virtual int pendingExtrinsics(GenericExtrinsic *buf, int bufferSize);

    virtual int subscribeBlockNumber(std::function<void(long long)> callback);
    virtual int unsubscribeBlockNumber();
    virtual int subscribeBalance(string address, std::function<void(uint128)> callback);
    virtual int unsubscribeBalance(string address);
    virtual int subscribeEraAndSession(std::function<void(Era, Session)> callback);
    virtual int unsubscribeEraAndSession();
    virtual int subscribeAccountNonce(string address, std::function<void(unsigned long)> callback);
    virtual int unsubscribeAccountNonce(string address);
};
