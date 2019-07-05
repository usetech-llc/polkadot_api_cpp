#include "polkadot.h"

// c++ 17 std::invoke implementation
template <class PMF, class Pointer, class... Args>
inline auto INVOKE(PMF pmf, Pointer &&ptr, Args &&... args)
    -> decltype(((*std::forward<Pointer>(ptr).*pmf)(std::forward<Args>(args)...))) {
    return ((*std::forward<Pointer>(ptr)).*pmf)(std::forward<Args>(args)...);
}

CPolkaApi::CPolkaApi(ILogger *logger, IJsonRpc *jsonRpc)
    : _blockNumberSubscriber(nullptr), _blockNumberSubscriptionId(0) {
    _logger = logger;
    _jsonRpc = jsonRpc;
}

int CPolkaApi::connect() {
    int result = PAPI_OK;

    // 1. Connect to WS
    result = _jsonRpc->connect();

    // 2. Read metadata for some block and initialize protocol parameters
    string blockHash = "0x37096ff58d1831c2ee64b026f8b70afab1942119c022d1dcfdbdc1558ebf63fa";
    unique_ptr<GetMetadataParams> prm(new GetMetadataParams);
    strcpy(prm->blockHash, blockHash.c_str());

    auto mdresp = getMetadata(move(prm));
    _protocolPrm.FreeBalanceHasher = getFuncHasher(mdresp, string("Balances"), string("FreeBalance"));
    _protocolPrm.FreeBalancePrefix = "Balances FreeBalance";

    return result;
}

Hasher CPolkaApi::getFuncHasher(unique_ptr<Metadata> &meta, const string &moduleName, const string &funcName) {
    Hasher hasher = XXHASH;
    if (meta->metadataV0) {
        hasher = XXHASH;
    } else if (meta->metadataV5) {
        // Find the module index in metadata
        int moduleIndex = -1;
        for (int i = 0; i < COLLECTION_SIZE; ++i) {
            if (moduleName == meta->metadataV5->module[i]->name) {
                moduleIndex = i;
                break;
            }
        }

        // Find function by name in module and get its hasher
        string hasherStr = "";
        if (moduleIndex >= 0) {
            for (int i = 0; i < COLLECTION_SIZE; ++i) {
                if (funcName == meta->metadataV5->module[moduleIndex]->storage[i].name) {
                    hasherStr = meta->metadataV5->module[moduleIndex]->storage[i].type.hasher;
                    break;
                }
            }
        }

        // Parse hasher name
        if (hasherStr == "Blake2_256") {
            hasher = BLAKE2;
        }
    }
    return hasher;
}

void CPolkaApi::disconnect() { _jsonRpc->disconnect(); }

/*
    Wrapper for every a business object creation operation
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

/*  Call 4 methods and put them together in a single object
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
    unique_ptr<SystemInfo> si(new SystemInfo);
    strcpy(si->chainName, jsonObject[0].string_value().c_str());
    strcpy(si->chainId, jsonObject[1].string_value().c_str());
    strcpy(si->version, jsonObject[2].string_value().c_str());
    si->tokenDecimals = jsonObject[3]["tokenDecimals"].int_value();
    strcpy(si->tokenSymbol, jsonObject[3]["tokenSymbol"].string_value().c_str());
    return si;
}

unique_ptr<BlockHash> CPolkaApi::createBlockHash(Json jsonObject) {
    unique_ptr<BlockHash> bh(new BlockHash);
    strcpy(bh->hash, jsonObject.string_value().c_str());
    return bh;
}

unique_ptr<RuntimeVersion> CPolkaApi::createRuntimeVersion(Json jsonObject) {
    unique_ptr<RuntimeVersion> rv(new RuntimeVersion);

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

unique_ptr<Metadata> CPolkaApi::createMetadata(Json jsonObject) {

    unique_ptr<Metadata> md(new Metadata);
    MetadataFactory mdf(_logger);

    // cut off "0x"
    mdf.setInputData(jsonObject.string_value().substr(2));

    md->metadataV0 = mdf.getMetadataV0();
    md->metadataV5 = mdf.getMetadataV5();

    return md;
}

unique_ptr<BlockHash> CPolkaApi::getBlockHash(unique_ptr<GetBlockHashParams> params) {

    Json query = Json::object{{"method", "chain_getBlockHash"}, {"params", Json::array{params->blockNumber}}};

    Json response = _jsonRpc->request(query);

    return move(deserialize<BlockHash, &CPolkaApi::createBlockHash>(response));
}

unique_ptr<Metadata> CPolkaApi::getMetadata(unique_ptr<GetMetadataParams> params) {

    Json query = Json::object{{"method", "state_getMetadata"}, {"params", Json::array{params->blockHash}}};

    Json response = _jsonRpc->request(query);

    return move(deserialize<Metadata, &CPolkaApi::createMetadata>(response));
}

unique_ptr<RuntimeVersion> CPolkaApi::getRuntimeVersion(unique_ptr<GetRuntimeVersionParams> params) {

    Json query = Json::object{{"method", "chain_getRuntimeVersion"}, {"params", Json::array{params->blockHash}}};

    Json response = _jsonRpc->request(query);

    return move(deserialize<RuntimeVersion, &CPolkaApi::createRuntimeVersion>(response));
}

template <typename T> T CPolkaApi::fromHex(string hexStr, bool bigEndianBytes) {
    int offset = 0;
    int byteOffset = 0;
    if ((hexStr[0] == '0') && (hexStr[1] == 'x')) {
        offset = 2;
    }
    T result = 0;
    while (offset < (int)hexStr.length()) {
        unsigned char digit1 = hexStr[offset];
        unsigned char digit2 = hexStr[offset + 1];
        unsigned char byte = 0;
        if ((digit1 >= 'a') && (digit1 <= 'f'))
            digit1 = digit1 - 'a' + 10;
        else if ((digit1 >= 'A') && (digit1 <= 'F'))
            digit1 = digit1 - 'A' + 10;
        else if ((digit1 >= '0') && (digit1 <= '9'))
            digit1 = digit1 - '0';
        if ((digit2 >= 'a') && (digit2 <= 'f'))
            digit2 = digit2 - 'a' + 10;
        else if ((digit2 >= 'A') && (digit2 <= 'F'))
            digit2 = digit2 - 'A' + 10;
        else if ((digit2 >= '0') && (digit2 <= '9'))
            digit2 = digit2 - '0';

        byte = (digit1 << 4) | digit2;

        if (bigEndianBytes) {
            result = (result << 8) | byte;
        } else {
            T wbyte = byte;
            result = (wbyte << byteOffset) | result;
            byteOffset += 8;
        }

        offset += 2;
    }
    return result;
}

void CPolkaApi::handleWsMessage(const int subscriptionId, const Json &message) {

    // Handle Block subscriptions
    if (_blockNumberSubscriptionId == subscriptionId) {
        _blockNumberSubscriber(fromHex<long long>(message["number"].string_value()));
        return;
    }

    // Handle Balance subscriptions
    for (auto const &sid : _balanceSubscriptionIds) {
        if (sid.second == subscriptionId) {
            _balanceSubscribers[sid.first](fromHex<unsigned __int128>(message["changes"][0][1].string_value(), false));
            return;
        }
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

int CPolkaApi::unsubscribeBlockNumber() {
    if (_blockNumberSubscriptionId) {
        _jsonRpc->unsubscribeWs(_blockNumberSubscriptionId);
        _blockNumberSubscriber = nullptr;
        _blockNumberSubscriptionId = 0;
    }
    return PAPI_OK;
}

int CPolkaApi::subscribeBalance(string address, std::function<void(unsigned __int128)> callback) {
    _balanceSubscribers[address] = callback;

    // Subscribe to websocket
    if (_balanceSubscriptionIds.count(address) == 0) {
        Address addrStruct;
        memcpy(addrStruct.symbols, "5FpxCaAovn3t2sTsbBeT5pWTj2rg392E8QoduwAyENcPrKht", ADDRESS_LENGTH);
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
