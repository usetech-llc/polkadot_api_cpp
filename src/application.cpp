#include "polkadot.h"

// c++ 17 std::invoke implementation
template <class PMF, class Pointer, class... Args>
inline auto INVOKE(PMF pmf, Pointer &&ptr, Args &&... args)
    -> decltype(((*std::forward<Pointer>(ptr).*pmf)(std::forward<Args>(args)...))) {
    return ((*std::forward<Pointer>(ptr)).*pmf)(std::forward<Args>(args)...);
}

CPolkaApi::CPolkaApi(ILogger *logger, IJsonRpc *jsonRpc) {
    _logger = logger;
    _jsonRpc = jsonRpc;
}

int CPolkaApi::connect() { return _jsonRpc->connect(); }

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

unique_ptr<BlockHash> CPolkaApi::getBlockHash(unique_ptr<GetBlockHashParams> params) {

    Json query = Json::object{{"method", "chain_getBlockHash"}, {"params", Json::array{params->blockNumber}}};

    Json response = _jsonRpc->request(query);

    return move(deserialize<BlockHash, &CPolkaApi::createBlockHash>(response));
}

unique_ptr<Metadata> CPolkaApi::getMetadata(unique_ptr<GetMetadataParams> params) {
    unique_ptr<Metadata> stub(new Metadata);
    return move(stub);
}

unique_ptr<RuntimeVersion> CPolkaApi::getRuntimeVersion(unique_ptr<GetRuntimeVersionParams> params) {

    Json query = Json::object{{"method", "chain_getRuntimeVersion"}, {"params", Json::array{params->blockHash}}};

    Json response = _jsonRpc->request(query);

    return move(deserialize<RuntimeVersion, &CPolkaApi::createRuntimeVersion>(response));
}
