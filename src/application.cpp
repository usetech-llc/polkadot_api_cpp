#include "polkadot.h"

// c++ 17 std::invoke implementation
template <class PMF, class Pointer, class... Args>
inline auto INVOKE(PMF pmf, Pointer &&ptr, Args &&... args)
    -> decltype(((*std::forward<Pointer>(ptr).*pmf)(std::forward<Args>(args)...))) {
    return ((*std::forward<Pointer>(ptr)).*pmf)(std::forward<Args>(args)...);
}

CPolkaApi::CPolkaApi(ILogger *logger, IJsonRpc *jsonRpc)
    : _blockNumberSubscriber(nullptr), _eraAndSessionSubscriber(nullptr), _blockNumberSubscriptionId(0),
      _eraAndSessionSubscriptionId(0), _bestBlockNum(-1) {
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
    cout << response.dump();

    return move(deserialize<Metadata, &CPolkaApi::createMetadata>(response));
}

unique_ptr<RuntimeVersion> CPolkaApi::getRuntimeVersion(unique_ptr<GetRuntimeVersionParams> params) {

    Json query = Json::object{{"method", "chain_getRuntimeVersion"}, {"params", Json::array{params->blockHash}}};

    Json response = _jsonRpc->request(query);

    return move(deserialize<RuntimeVersion, &CPolkaApi::createRuntimeVersion>(response));
}

string CPolkaApi::reverseBytes(string str) {
    if (strcmp(str.substr(0, 2).c_str(), "0x") == 0)
        str = str.substr(2);

    string result;
    for (unsigned int i = 0; i < str.length(); i += 2) {

        auto bt = str.substr(i, 2);
        result.insert(0, bt);
    }
    return result;
}

long long CPolkaApi::fromHex(string hexStr) {
    int offset = 0;
    if ((hexStr[0] == '0') && (hexStr[1] == 'x')) {
        offset = 2;
    }
    long long result = 0;
    while (offset < (int)hexStr.length()) {
        unsigned char digit = hexStr[offset];
        if ((digit >= 'a') && (digit <= 'f'))
            digit = digit - 'a' + 10;
        else if ((digit >= 'A') && (digit <= 'F'))
            digit = digit - 'A' + 10;
        else if ((digit >= '0') && (digit <= '9'))
            digit = digit - '0';
        result = (result << 4) | digit;
        offset++;
    }
    return result;
}

void CPolkaApi::handleWsMessage(const int subscriptionId, const Json &message) {

    if (_blockNumberSubscriptionId == subscriptionId) {
        _blockNumberSubscriber(fromHex(message["number"].string_value()));
    }

    if (_eraAndSessionSubscriptionId == subscriptionId && _bestBlockNum != -1) {

        cout << endl << endl << endl << endl << endl << "value: " << (message.dump());

        auto lastLengthChange = fromHex(reverseBytes(message["changes"][0][1].string_value()));
        auto sessionLength = fromHex(reverseBytes(message["changes"][1][1].string_value()));
        auto currentEra = fromHex(reverseBytes(message["changes"][2][1].string_value()));
        auto sessionsPerEra = fromHex(reverseBytes(message["changes"][3][1].string_value()));
        auto currentIndexSubcription = fromHex(reverseBytes(message["changes"][4][1].string_value()));

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
    auto params = Json::array{Json::array{CConstants::lastLengthChangeSubcription, CConstants::sessionLengthSubcription,
                                          CConstants::currentEraSubcription, CConstants::sessionsPerEraSubcription,
                                          CConstants::currentIndexSubcription}};

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
