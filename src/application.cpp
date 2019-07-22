#include "polkadot.h"

// c++ 17 std::invoke implementation
template <class PMF, class Pointer, class... Args>
inline auto INVOKE(PMF pmf, Pointer &&ptr, Args &&... args)
    -> decltype(((*std::forward<Pointer>(ptr).*pmf)(std::forward<Args>(args)...))) {
    return ((*std::forward<Pointer>(ptr)).*pmf)(std::forward<Args>(args)...);
}

CPolkaApi::CPolkaApi(ILogger *logger, IJsonRpc *jsonRpc)
    : _bestBlockNum(-1), _blockNumberSubscriber(nullptr), _eraAndSessionSubscriber(nullptr),
      _blockNumberSubscriptionId(0), _eraAndSessionSubscriptionId(0) {
    _logger = logger;
    _jsonRpc = jsonRpc;
}

int CPolkaApi::connect(string node_url) {
    int result = PAPI_OK;

    // 1. Connect to WS
    result = _jsonRpc->connect(node_url);

    // 2. Read last block hash
    unique_ptr<GetBlockHashParams> par(new GetBlockHashParams);
    par->blockNumber = 0;
    auto genesisHashStr = getBlockHash(move(par));
    for (int i = 0; i < BLOCK_HASH_SIZE; ++i) {
        _protocolPrm.GenesisBlockHash[i] = fromHexByte(genesisHashStr->hash + 2 + i * 2);
    }

    // 3. Read metadata for head block and initialize protocol parameters
    auto mdresp = getMetadata(nullptr);
    _protocolPrm.FreeBalanceHasher = getFuncHasher(mdresp, string("Balances"), string("FreeBalance"));
    _protocolPrm.FreeBalancePrefix = "Balances FreeBalance";
    _protocolPrm.BalanceModuleIndex = 3; // getModuleIndex(mdresp, string("Balances"), true);
    _protocolPrm.TransferMethodIndex =
        getCallMethodIndex(mdresp, getModuleIndex(mdresp, string("Balances"), false), string("transfer"));

    if (_protocolPrm.FreeBalanceHasher == XXHASH)
        _logger->info("FreeBalance hash function is xxHash");
    else
        _logger->info("FreeBalance hash function is Blake2-256");

    _logger->info(string("Balances module index: ") + to_string(_protocolPrm.BalanceModuleIndex));
    _logger->info(string("Transfer call index: ") + to_string(_protocolPrm.TransferMethodIndex));

    return result;
}

char easytolower(char in) {
    if (in <= 'Z' && in >= 'A')
        return in - ('Z' - 'z');
    return in;
}

Hasher CPolkaApi::getFuncHasher(unique_ptr<Metadata> &meta, const string &moduleName, const string &funcName) {
    Hasher hasher = XXHASH;

    if (meta->metadataV0) {
        hasher = XXHASH;
    } else {
        // Find the module index in metadata
        int moduleIndex = getModuleIndex(meta, moduleName, false);

        // Find function by name in module and get its hasher
        string hasherStr = "";
        if (moduleIndex >= 0) {
            int methodIndex = getStorageMethodIndex(meta, moduleIndex, funcName);
            if (methodIndex > 0) {
                if (meta->metadataV4)
                    hasherStr = meta->metadataV4->module[moduleIndex]->storage[methodIndex]->type.hasher;
                else if (meta->metadataV5)
                    hasherStr = meta->metadataV5->module[moduleIndex]->storage[methodIndex]->type.hasher;
                else if (meta->metadataV6)
                    hasherStr = meta->metadataV6->module[moduleIndex]->storage[methodIndex]->type.hasher;
            }
        }

        // Parse hasher name
        if (hasherStr == "Blake2_256") {
            hasher = BLAKE2;
        }
    }
    return hasher;
}

int CPolkaApi::getModuleIndex(unique_ptr<Metadata> &meta, const string &moduleName, bool skipZeroCalls) {

    // Find the module index in metadata
    int moduleIndex = -1;
    int zeroMethodModuleCount = 0;
    string moduleNameLower = moduleName;
    std::transform(moduleNameLower.begin(), moduleNameLower.end(), moduleNameLower.begin(), easytolower);

    for (int i = 0; i < COLLECTION_SIZE; ++i) {
        string name("");
        if (meta->metadataV0 && meta->metadataV0->module[i]) {
            name = meta->metadataV0->module[i]->prefix;
        } else if (meta->metadataV4 && meta->metadataV4->module[i]) {
            name = meta->metadataV4->module[i]->name;
        } else if (meta->metadataV5 && meta->metadataV5->module[i]) {
            name = meta->metadataV5->module[i]->name;
        } else if (meta->metadataV6 && meta->metadataV6->module[i]) {
            name = meta->metadataV6->module[i]->name;
        }

        if (name.length()) {
            std::transform(name.begin(), name.end(), name.begin(), easytolower);
            if (!hasMethods(meta, i))
                zeroMethodModuleCount++;

            if (moduleNameLower == name) {
                moduleIndex = i;
                if (skipZeroCalls) {
                    moduleIndex -= zeroMethodModuleCount;
                }
                break;
            }
        }
    }

    return moduleIndex;
}

int CPolkaApi::getStorageMethodIndex(unique_ptr<Metadata> &meta, const int moduleIndex, const string &funcName) {
    int methodIndex = -1;
    string funcNameLower = funcName;
    std::transform(funcNameLower.begin(), funcNameLower.end(), funcNameLower.begin(), easytolower);

    for (int i = 0; i < COLLECTION_SIZE; ++i) {

        string name("");
        if (meta->metadataV0 && meta->metadataV0->module[i]) {
            name = meta->metadataV0->module[moduleIndex]->storage.function[i].name;
        } else if (meta->metadataV4 && meta->metadataV4->module[i]) {
            name = meta->metadataV4->module[moduleIndex]->storage[i]->name;
        } else if (meta->metadataV5 && meta->metadataV5->module[i]) {
            name = meta->metadataV5->module[moduleIndex]->storage[i]->name;
        } else if (meta->metadataV6 && meta->metadataV6->module[i]) {
            name = meta->metadataV6->module[moduleIndex]->storage[i]->name;
        }

        std::transform(name.begin(), name.end(), name.begin(), easytolower);

        if (funcNameLower == name) {
            methodIndex = i;
            break;
        }
    }

    return methodIndex;
}

int CPolkaApi::getCallMethodIndex(unique_ptr<Metadata> &meta, const int moduleIndex, const string &funcName) {
    int methodIndex = -1;
    string funcNameLower = funcName;
    std::transform(funcNameLower.begin(), funcNameLower.end(), funcNameLower.begin(), easytolower);

    for (int i = 0; i < COLLECTION_SIZE; ++i) {

        string name("");
        if (meta->metadataV0 && meta->metadataV0->module[i]) {
            name = meta->metadataV0->module[moduleIndex]->module.call.fn1[i].name;
        } else if (meta->metadataV4 && meta->metadataV4->module[i]) {
            name = meta->metadataV4->module[moduleIndex]->call[i]->name;
        } else if (meta->metadataV5 && meta->metadataV5->module[i]) {
            name = meta->metadataV5->module[moduleIndex]->call[i]->name;
        } else if (meta->metadataV6 && meta->metadataV6->module[i]) {
            name = meta->metadataV6->module[moduleIndex]->call[i]->name;
        }

        std::transform(name.begin(), name.end(), name.begin(), easytolower);

        if (funcNameLower == name) {
            methodIndex = i;
            break;
        }
    }

    return methodIndex;
}

bool CPolkaApi::hasMethods(unique_ptr<Metadata> &meta, const int moduleIndex) {
    for (int i = 0; i < COLLECTION_SIZE; ++i) {

        // Check call methods
        if (meta->metadataV0 && meta->metadataV0->module[i]) {
            if (strlen(meta->metadataV0->module[moduleIndex]->module.call.fn1[i].name))
                return true;
        } else if (meta->metadataV4 && meta->metadataV4->module[i]) {
            return (meta->metadataV4->module[moduleIndex]->call[i] != nullptr);
        } else if (meta->metadataV5 && meta->metadataV5->module[i]) {
            return (meta->metadataV5->module[moduleIndex]->call[i] != nullptr);
        } else if (meta->metadataV6 && meta->metadataV6->module[i]) {
            return (meta->metadataV6->module[moduleIndex]->call[i] != nullptr);
        }
    }

    return false;
}

void CPolkaApi::disconnect() { _jsonRpc->disconnect(); }

/**
 *    Wrapper for every business object creation operation
 */
template <typename T, unique_ptr<T> (CPolkaApi::*F)(Json)> unique_ptr<T> CPolkaApi::deserialize(Json jsonObject) {

    try {
        // invoke pointer-to-member function
        return INVOKE(F, this, jsonObject);
    } catch (std::exception &e) {
        string errstr("Cannot deserialize data ");
        _logger->error(errstr + e.what());
        throw ApplicationException(errstr + e.what());
        return nullptr;
    }
}

/**  Call 4 methods and put them together in a single object
 *  system_chain
 *  system_name
 *  system_version
 *  system_properties
 */
unique_ptr<SystemInfo> CPolkaApi::getSystemInfo() {

    Json systemNameQuery = Json::object{{"method", "system_name"}, {"params", Json::array{}}};
    Json systemNameJson = _jsonRpc->request(systemNameQuery);

    Json systemChainQuery = Json::object{{"method", "system_chain"}, {"params", Json::array{}}};
    Json systemChainJson = _jsonRpc->request(systemChainQuery);

    Json systemVersionQuery = Json::object{{"method", "system_version"}, {"params", Json::array{}}};
    Json systemVersionJson = _jsonRpc->request(systemVersionQuery);

    Json systemPropertiesQuery = Json::object{{"method", "system_properties"}, {"params", Json::array{}}};
    Json systemPropertiesJson = _jsonRpc->request(systemPropertiesQuery);

    Json completeJson = Json::array{systemNameJson, systemChainJson, systemVersionJson, systemPropertiesJson};

    return move(deserialize<SystemInfo, &CPolkaApi::createSystemInfo>(completeJson));
}

unique_ptr<SystemInfo> CPolkaApi::createSystemInfo(Json jsonObject) {
    SystemInfo *result = new SystemInfo();
    memset(result, 0, sizeof(SystemInfo));
    unique_ptr<SystemInfo> si(result);
    strcpy(si->chainName, jsonObject[0].string_value().c_str());
    strcpy(si->chainId, jsonObject[1].string_value().c_str());
    strcpy(si->version, jsonObject[2].string_value().c_str());
    si->tokenDecimals = jsonObject[3]["tokenDecimals"].int_value();
    strcpy(si->tokenSymbol, jsonObject[3]["tokenSymbol"].string_value().c_str());
    return si;
}

unique_ptr<BlockHash> CPolkaApi::createBlockHash(Json jsonObject) {
    BlockHash *result = new BlockHash();
    memset(result, 0, sizeof(BlockHash));
    unique_ptr<BlockHash> bh(result);
    strcpy(bh->hash, jsonObject.string_value().c_str());
    return bh;
}

unique_ptr<RuntimeVersion> CPolkaApi::createRuntimeVersion(Json jsonObject) {
    RuntimeVersion *result = new RuntimeVersion();
    memset(result, 0, sizeof(RuntimeVersion));
    unique_ptr<RuntimeVersion> rv(result);
    strcpy(rv->specName, jsonObject["specName"].string_value().c_str());
    strcpy(rv->implName, jsonObject["implName"].string_value().c_str());
    rv->authoringVersion = jsonObject["authoringVersion"].int_value();
    rv->specVersion = jsonObject["specVersion"].int_value();
    rv->implVersion = jsonObject["implVersion"].int_value();

    int i = 0;
    for (Json item : jsonObject["apis"].array_items()) {
        strcpy(rv->api[i].num, item[0].string_value().c_str());
        rv->api[i].id = item[1].int_value();
        i++;
    }
    return rv;
}

unique_ptr<SignedBlock> CPolkaApi::createBlock(Json jsonObject) {

    SignedBlock *result = new SignedBlock();
    memset(result, 0, sizeof(SignedBlock));
    unique_ptr<SignedBlock> sb(result);

    strcpy(sb->block.header.parentHash, jsonObject["block"]["header"]["parentHash"].string_value().c_str());
    sb->block.header.number = (unsigned long long) atoi128(jsonObject["block"]["header"]["parentHash"].string_value().substr(2).c_str());
    strcpy(sb->block.header.stateRoot, jsonObject["block"]["header"]["stateRoot"].string_value().c_str());
    strcpy(sb->block.header.extrinsicsRoot, jsonObject["block"]["header"]["extrinsicsRoot"].string_value().c_str());

    int i = 0;
    for (Json item : jsonObject["block"]["header"]["digest"]["logs"].array_items()) {
        strcpy(sb->block.header.digest[i].value, item.string_value().c_str());
        i++;
    }

    i = 0;
    for (Json item : jsonObject["block"]["extrinsics"].array_items()) {
        strcpy(sb->block.extrinsic[i], item.string_value().c_str());
        i++;
    }

    memcpy(sb->justification, &jsonObject["justification"], sizeof(jsonObject["justification"]));

    return sb;
}

unique_ptr<BlockHeader> CPolkaApi::createBlockHeader(Json jsonObject) {

    BlockHeader *result = new BlockHeader();
    memset(result, 0, sizeof(BlockHeader));
    unique_ptr<BlockHeader> bh(result);

    strcpy(bh->parentHash, jsonObject["parentHash"].string_value().c_str());
    bh->number = (unsigned long long) atoi128(jsonObject["parentHash"].string_value().substr(2).c_str());
    strcpy(bh->stateRoot, jsonObject["stateRoot"].string_value().c_str());
    strcpy(bh->extrinsicsRoot, jsonObject["extrinsicsRoot"].string_value().c_str());

    int i = 0;
    for (Json item : jsonObject["digest"]["logs"].array_items()) {
        strcpy(bh->digest[i].value, item.string_value().c_str());
        i++;
    }

    return bh;
}

unique_ptr<FinalHead> CPolkaApi::createFinalHead(Json jsonObject) {

    FinalHead *result = new FinalHead();
    memset(result, 0, sizeof(FinalHead));
    unique_ptr<FinalHead> fh(result);

    strcpy(fh->blockHash, jsonObject.string_value().c_str());

    return fh;
}

unique_ptr<SystemHealth> CPolkaApi::createSystemHealth(Json jsonObject) {

    SystemHealth *result = new SystemHealth();
    memset(result, 0, sizeof(SystemHealth));
    unique_ptr<SystemHealth> sh(result);

    sh->peers = jsonObject["peers"].int_value();
    sh->isSyncing = jsonObject["isSyncing"].bool_value();
    sh->shouldHavePeers = jsonObject["shouldHavePeers"].bool_value();

    return sh;
}

unique_ptr<NetworkState> CPolkaApi::createNetworkState(Json jsonObject) {
    NetworkState *result = new NetworkState();
    memset(result, 0, sizeof(NetworkState));
    unique_ptr<NetworkState> ns(result);

    ns->AverageDownloadPerSec = jsonObject["averageDownloadPerSec"].int_value();
    ns->AverageUploadPerSec = jsonObject["averageUploadPerSec"].int_value();

    // parse items as map<string, Json>
    int i = 0;
    for (auto const &item : jsonObject["connectedPeers"].object_items()) {

        strcpy(ns->connectedPeers[i].key, item.first.c_str());
        ns->connectedPeers[i].connectedPeerInfo.enabled = item.second["enabled"].bool_value();

        // endpoint -> dealing
        Endpoint ep;
        strcpy(ep.dialing, item.second["dialing"].string_value().c_str());
        ns->connectedPeers[i].connectedPeerInfo.endpoint = ep;

        // knownAddresses
        int kai = 0;
        for (Json kaitem : item.second["knownAddresses"].array_items()) {
            strcpy(ns->connectedPeers[i].connectedPeerInfo.knownAddresses[kai], kaitem.string_value().c_str());
            kai++;
        }

        // latestPingTime
        ConnectedPeerTime cpt;
        cpt.nanos = item.second["latestPingTime"]["nanos"].int_value();
        cpt.secs = item.second["latestPingTime"]["secs"].int_value();
        ns->connectedPeers[i].connectedPeerInfo.latestPingTime = cpt;

        ns->connectedPeers[i].connectedPeerInfo.open = item.second["open"].bool_value();

        strcpy(ns->connectedPeers[i].connectedPeerInfo.versionString,
               item.second["versionString"].string_value().c_str());

        i++;
    }

    i = 0;
    for (Json item : jsonObject["externalAddresses"].array_items()) {
        strcpy(ns->externalAddresses[i], item.string_value().c_str());
        i++;
    }

    i = 0;
    for (Json item : jsonObject["listenedAddresses"].array_items()) {
        strcpy(ns->listenedAddresses[i], item.string_value().c_str());
        i++;
    }

    i = 0;
    for (auto const &item : jsonObject["notConnectedPeers"].object_items()) {

        strcpy(ns->notConnectedPeers[i].key, item.first.c_str());

        // knownAddresses
        int kai = 0;
        for (Json kaitem : item.second["knownAddresses"].array_items()) {
            strcpy(ns->notConnectedPeers[i].notConnectedPeerInfo.knownAddresses[kai], kaitem.string_value().c_str());
            kai++;
        }
    }

    strcpy(ns->peerId, jsonObject["peerId"].string_value().c_str());
    strcpy(ns->peerset, jsonObject["peerset"].string_value().c_str());

    return ns;
}

unique_ptr<Metadata> CPolkaApi::createMetadata(Json jsonObject) {
    unique_ptr<Metadata> md(new Metadata);
    MetadataFactory mdf(_logger);

    // cut off "0x"
    mdf.setInputData(jsonObject.string_value().substr(2));

    md->metadataV0 = mdf.getMetadataV0();
    md->metadataV4 = mdf.getMetadataV4();
    md->metadataV5 = mdf.getMetadataV5();
    md->metadataV6 = mdf.getMetadataV6();

    return md;
}

unique_ptr<PeersInfo> CPolkaApi::createPeerInfo(Json jsonObject) {

    PeersInfo *result = new PeersInfo();
    memset(result, 0, sizeof(PeersInfo));
    unique_ptr<PeersInfo> pi(result);

    int i = 0;
    for (Json item : jsonObject.array_items()) {

        strcpy(pi->peers[i].bestHash, item["bestHash"].string_value().c_str());
        pi->peers[i].bestNumber = item["betNumber"].int_value();
        strcpy(pi->peers[i].peerId, item["peerId"].string_value().c_str());
        pi->peers[i].protocolVersion = item["protocolVersion"].int_value();
        strcpy(pi->peers[i].roles, item["roles"].string_value().c_str());
        i++;
    }

    return pi;
}

unique_ptr<BlockHash> CPolkaApi::getBlockHash(unique_ptr<GetBlockHashParams> params) {

    Json prm = Json::array{};
    if (params)
        prm = Json::array{params->blockNumber};
    Json query = Json::object{{"method", "chain_getBlockHash"}, {"params", prm}};

    Json response = _jsonRpc->request(query);

    return move(deserialize<BlockHash, &CPolkaApi::createBlockHash>(response));
}

unique_ptr<Metadata> CPolkaApi::getMetadata(unique_ptr<GetMetadataParams> params) {

    Json prm = Json::array{};
    if (params)
        prm = Json::array{params->blockHash};
    Json query = Json::object{{"method", "state_getMetadata"}, {"params", prm}};

    Json response = _jsonRpc->request(query);

    return move(deserialize<Metadata, &CPolkaApi::createMetadata>(response));
}

unique_ptr<RuntimeVersion> CPolkaApi::getRuntimeVersion(unique_ptr<GetRuntimeVersionParams> params) {

    Json prm = Json::array{};
    if (params)
        prm = Json::array{params->blockHash};
    Json query = Json::object{{"method", "chain_getRuntimeVersion"}, {"params", prm}};

    Json response = _jsonRpc->request(query);

    return move(deserialize<RuntimeVersion, &CPolkaApi::createRuntimeVersion>(response));
}

unique_ptr<SignedBlock> CPolkaApi::getBlock(unique_ptr<GetBlockParams> params) {

    Json prm = Json::array{};
    if (params)
        prm = Json::array{params->blockHash};
    Json query = Json::object{{"method", "chain_getBlock"}, {"params", prm}};

    Json response = _jsonRpc->request(query);

    return move(deserialize<SignedBlock, &CPolkaApi::createBlock>(response));
}

unique_ptr<BlockHeader> CPolkaApi::getBlockHeader(unique_ptr<GetBlockParams> params) {

    Json prm = Json::array{};
    if (params)
        prm = Json::array{params->blockHash};
    Json query = Json::object{{"method", "chain_getHeader"}, {"params", prm}};

    Json response = _jsonRpc->request(query);

    return move(deserialize<BlockHeader, &CPolkaApi::createBlockHeader>(response));
}

unique_ptr<SystemHealth> CPolkaApi::getSystemHealth() {

    Json query = Json::object{{"method", "system_health"}, {"params", Json::array()}};

    Json response = _jsonRpc->request(query);

    return move(deserialize<SystemHealth, &CPolkaApi::createSystemHealth>(response));
}

unique_ptr<FinalHead> CPolkaApi::getFinalizedHead() {

    Json query = Json::object{{"method", "chain_getFinalizedHead"}, {"params", Json::array()}};

    Json response = _jsonRpc->request(query);

    return move(deserialize<FinalHead, &CPolkaApi::createFinalHead>(response));
}

unique_ptr<PeersInfo> CPolkaApi::getSystemPeers() {

    Json query = Json::object{{"method", "system_peers"}, {"params", Json::array()}};

    Json response = _jsonRpc->request(query);

    return move(deserialize<PeersInfo, &CPolkaApi::createPeerInfo>(response));
}

unique_ptr<NetworkState> CPolkaApi::getNetworkState() {

    Json query = Json::object{{"method", "system_networkState"}, {"params", Json::array()}};

    Json response = _jsonRpc->request(query);

    return move(deserialize<NetworkState, &CPolkaApi::createNetworkState>(response));
}

unsigned long CPolkaApi::getAccountNonce(string address) {
    // Subscribe to account nonce updates and immediately unsubscribe after response is received
    // This pattern is borrowed from POC-3 UI
    bool done = false;
    unsigned long result = 0;
    subscribeAccountNonce(address, [&](unsigned long nonce) {
        result = nonce;
        done = true;
    });

    while (!done)
        usleep(10000);

    unsubscribeAccountNonce(address);

    return result;
}

void CPolkaApi::handleWsMessage(const int subscriptionId, const Json &message) {

    // TODO: DOT-55, fix with proper producer-consumer
    int count = 0;
    while (count < 10) {
        // Handle Block subscriptions
        if (_blockNumberSubscriptionId == subscriptionId) {
            _blockNumberSubscriber(fromHex<long long>(message["number"].string_value()));
            return;
        }

        // Handle Account nonce subscriptions
        for (auto const &sid : _nonceSubscriptionIds) {
            if (sid.second == subscriptionId) {
                _nonceSubscribers[sid.first](fromHex<unsigned long>(message["changes"][0][1].string_value(), false));
                return;
            }
        }

        // Handle Balance subscriptions
        for (auto const &sid : _balanceSubscriptionIds) {
            if (sid.second == subscriptionId) {
                _balanceSubscribers[sid.first](fromHex<uint128>(message["changes"][0][1].string_value(), false));
                return;
            }
        }

        // Handle transaction completion subscriptions
        if (_transactionCompletionSubscriptionId == subscriptionId) {
            if (message.dump().find("ready") != std::string::npos)
                _transactionCompletionSubscriber(string("ready"));
            else if (message.dump().find("finalized") != std::string::npos) {
                _transactionCompletionSubscriber(string("finalized"));

                // There is no need to unsubscribe, just reset variabled
                _transactionCompletionSubscriber = nullptr;
                _transactionCompletionSubscriptionId = 0;
            }
            return;
        }

        if (_eraAndSessionSubscriptionId == subscriptionId && _bestBlockNum != -1) {
            auto lastLengthChange = fromHex<long long>(message["changes"][0][1].string_value(), false);
            auto sessionLength = fromHex<long long>(message["changes"][1][1].string_value(), false);
            auto currentEra = fromHex<long long>(message["changes"][2][1].string_value(), false);
            auto sessionsPerEra = fromHex<long long>(message["changes"][3][1].string_value(), false);
            auto currentIndexSubcription = fromHex<long long>(message["changes"][4][1].string_value(), false);

            auto sessionProgress = (_bestBlockNum - lastLengthChange + sessionLength) % sessionLength;
            auto eraProgress = currentIndexSubcription % sessionsPerEra * sessionLength + sessionProgress;

            Era era;
            era.currentEra = currentEra;
            era.eraProgress = eraProgress;
            Session session;
            session.sessionIndex = currentIndexSubcription;
            session.lastLengthChange = lastLengthChange;
            session.sessionLength = sessionLength;
            session.sessionProgress = sessionProgress;

            _eraAndSessionSubscriber(era, session);
        }

        usleep(100000);
        count++;
    }
}

int CPolkaApi::subscribeBlockNumber(std::function<void(long long)> callback) {
    _blockNumberSubscriber = callback;

    // Subscribe to websocket
    if (!_blockNumberSubscriptionId) {
        Json subscribeQuery = Json::object{{"method", "chain_subscribeNewHead"}, {"params", Json::array{}}};
        _blockNumberSubscriptionId = _jsonRpc->subscribeWs(subscribeQuery, this);
    }

    return PAPI_OK;
}

int CPolkaApi::subscribeEraAndSession(std::function<void(Era, Session)> callback) {
    _eraAndSessionSubscriber = callback;

    subscribeBlockNumber([&](long long blockNum) { _bestBlockNum = blockNum; });

    // era and session subscription
    auto params =
        Json::array{Json::array{LAST_LENGTH_CHANGE_SUBSCRIPTION, SESSION_LENGTH_SUBSCRIPTION, CURRENT_ERA_SUBSCRIPTION,
                                SESSIONS_PER_ERA_SUBSCRIPTION, CURRENT_INDEX_SUBSCRIPTION}};

    // Subscribe to websocket
    if (!_eraAndSessionSubscriptionId) {
        Json subscribeQuery = Json::object{{"method", "state_subscribeStorage"}, {"params", params}};
        _eraAndSessionSubscriptionId = _jsonRpc->subscribeWs(subscribeQuery, this);
    }

    return PAPI_OK;
}

int CPolkaApi::unsubscribeEraAndSession() {
    if (_eraAndSessionSubscriptionId) {
        _jsonRpc->unsubscribeWs(_eraAndSessionSubscriptionId);
        _eraAndSessionSubscriber = nullptr;
        _eraAndSessionSubscriptionId = 0;
    }
    unsubscribeBlockNumber();
    return PAPI_OK;
}

int CPolkaApi::unsubscribeBlockNumber() {
    if (_blockNumberSubscriptionId) {
        _jsonRpc->unsubscribeWs(_blockNumberSubscriptionId);
        _blockNumberSubscriber = nullptr;
        _blockNumberSubscriptionId = 0;
    }
    return PAPI_OK;
}

int CPolkaApi::subscribeBalance(string address, std::function<void(uint128)> callback) {
    _balanceSubscribers[address] = callback;

    // Subscribe to websocket
    if (_balanceSubscriptionIds.count(address) == 0) {
        Address addrStruct;
        memcpy(addrStruct.symbols, address.c_str(), ADDRESS_LENGTH);
        string storageKey =
            StorageUtils::getAddressStorageKey(_protocolPrm.FreeBalanceHasher, addrStruct, "Balances FreeBalance");
        Json subscribeQuery =
            Json::object{{"method", "state_subscribeStorage"}, {"params", Json::array{Json::array{storageKey}}}};
        _balanceSubscriptionIds[address] = _jsonRpc->subscribeWs(subscribeQuery, this);
    }

    return PAPI_OK;
}

int CPolkaApi::unsubscribeBalance(string address) {
    if (_balanceSubscriptionIds.count(address) == 0) {
        _jsonRpc->unsubscribeWs(_balanceSubscriptionIds[address]);
        _balanceSubscribers[address] = nullptr;
        _balanceSubscriptionIds.erase(address);
    }
    return PAPI_OK;
}

int CPolkaApi::subscribeAccountNonce(string address, std::function<void(unsigned long)> callback) {
    _nonceSubscribers[address] = callback;

    // Subscribe to websocket
    if (_nonceSubscriptionIds.count(address) == 0) {
        Address accountAddr;
        memcpy(accountAddr.symbols, address.c_str(), ADDRESS_LENGTH);

        auto storageKey = StorageUtils::getAddressStorageKey(_protocolPrm.FreeBalanceHasher, accountAddr,
                                                             string("System AccountNonce"));
        _logger->info(string("Nonce subscription storageKey: ") + storageKey);

        Json subscribeQuery =
            Json::object{{"method", "state_subscribeStorage"}, {"params", Json::array{Json::array{storageKey}}}};
        _nonceSubscriptionIds[address] = _jsonRpc->subscribeWs(subscribeQuery, this);
    }

    return PAPI_OK;
}

int CPolkaApi::unsubscribeAccountNonce(string address) {
    if (_nonceSubscriptionIds.count(address) == 0) {
        _jsonRpc->unsubscribeWs(_nonceSubscriptionIds[address]);
        _nonceSubscribers[address] = nullptr;
        _nonceSubscriptionIds.erase(address);
    }
    return PAPI_OK;
}

void CPolkaApi::signAndSendTransfer(string sender, string privateKey, string recipient, uint128 amount,
                                    std::function<void(string)> callback) {

    _logger->info("=== Starting a Transfer Extrinsic ===");

    // Get account Nonce
    unsigned long nonce = getAccountNonce(sender);
    _logger->info(string("sender nonce: ") + to_string(nonce));

    // Format transaction
    TransferExtrinsic te;
    memset(&te, 0, sizeof(te));
    te.method.moduleIndex = _protocolPrm.BalanceModuleIndex;
    te.method.methodIndex = _protocolPrm.TransferMethodIndex;
    auto recipientPK = AddressUtils::getPublicKeyFromAddr(recipient);
    memcpy(te.method.receiverPublicKey, recipientPK.bytes, PUBLIC_KEY_LENGTH);
    te.method.amount = amount;
    te.signature.version = SIGNATURE_VERSION;
    auto senderPK = AddressUtils::getPublicKeyFromAddr(sender);
    memcpy(te.signature.signerPublicKey, senderPK.bytes, PUBLIC_KEY_LENGTH);
    te.signature.nonce = nonce;
    te.signature.era = IMMORTAL_ERA;

    // Format signature payload
    SignaturePayload sp;
    sp.nonce = nonce;
    uint8_t methodBytes[MAX_METHOD_BYTES_SZ];
    sp.methodBytesLength = te.serializeMethodBinary(methodBytes);
    sp.methodBytes = methodBytes;
    sp.era = IMMORTAL_ERA;
    memcpy(sp.authoringBlockHash, _protocolPrm.GenesisBlockHash, BLOCK_HASH_SIZE);

    // Serialize and Sign payload
    uint8_t signaturePayloadBytes[MAX_METHOD_BYTES_SZ];
    long payloadLength = sp.serializeBinary(signaturePayloadBytes);

    vector<uint8_t> secretKeyVec = fromHex<vector<uint8_t>>(privateKey);
    uint8_t sig[SR25519_SIGNATURE_SIZE] = {0};
    sr25519_sign(sig, te.signature.signerPublicKey, secretKeyVec.data(), signaturePayloadBytes, payloadLength);

    // Copy signature bytes to transaction
    memcpy(te.signature.sr25519Signature, sig, SR25519_SIGNATURE_SIZE);

    // Serialize and send transaction
    uint8_t teBytes[MAX_METHOD_BYTES_SZ];
    long teByteLength = te.serializeBinary(teBytes);
    string teStr("0x");
    for (int i = 0; i < teByteLength; ++i) {
        char b[3] = {0};
        sprintf(b, "%02X", teBytes[i]);
        teStr += b;
    }

    Json query = Json::object{{"method", "author_submitAndWatchExtrinsic"}, {"params", Json::array{teStr}}};

    // Send == Subscribe callback to completion
    _transactionCompletionSubscriber = callback;
    _transactionCompletionSubscriptionId = _jsonRpc->subscribeWs(query, this);
}
