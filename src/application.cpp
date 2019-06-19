#include "polkadot.h"

CPolkaApi::CPolkaApi(){

    JsonRpcParams params;
    params.jsonrpcVersion = "2.0";

    _logger = new LoggerStub();
    _jsonRpc = new CJsonRpc(CWebSocketClient::getInstance(_logger), _logger, params);
}

CPolkaApi::~CPolkaApi(){
    delete _logger;
    delete _jsonRpc;
}

int CPolkaApi::connect(){
    _jsonRpc->connect();
}

void CPolkaApi::disconnect(){
    _jsonRpc->disconnect();
}

/*  Call 4 methods and put them together in a single object 
    *  system_chain
    *  system_name
    *  system_version
    *  system_properties
*/
unique_ptr<SystemInfo> CPolkaApi::getSystemInfo(){

    Json systemNameQuery = Json::object{
        {"method", "system_name"}, 
        {"params", Json::array{}}
        };
    Json systemNameJson = _jsonRpc->request(systemNameQuery);

    Json systemChainQuery = Json::object{
        {"method", "system_chain"}, 
        {"params", Json::array{}}
        };
    Json systemChainJson = _jsonRpc->request(systemChainQuery);

    Json systemVersionQuery = Json::object{
        {"method", "system_version"}, 
        {"params", Json::array{}}
        };
    Json systemVersionJson = _jsonRpc->request(systemVersionQuery);

    Json systemPropertiesQuery = Json::object{
        {"method", "system_properties"}, 
        {"params", Json::array{}}
        };
    Json systemPropertiesJson = _jsonRpc->request(systemPropertiesQuery);

    unique_ptr<SystemInfo> si(new SystemInfo);

    try{
        strcpy(si->chainName, systemNameJson.string_value().c_str());
        strcpy(si->chainId, systemChainJson.string_value().c_str());
        strcpy(si->version, systemVersionJson.string_value().c_str());
        si->tokenDecimals = systemPropertiesJson["tokenDecimals"].int_value();
        strcpy(si->tokenSymbol, systemPropertiesJson["tokenSymbol"].string_value().c_str());
    }
    catch(std::exception &e){
        string errstr("Cannot deserialize data ");
        _logger->error(errstr + e.what());
        throw ApplicationException(errstr + e.what());
        return nullptr;
    }

    return si;
}

unique_ptr<BlockHash> CPolkaApi::getBlockHash(unique_ptr<GetBlockHashParams> params){

    Json query = Json::object{
        {"method", "chain_getBlockHash"}, 
        {"params", Json::array{params->blockNumber}}
        };

    Json response = _jsonRpc->request(query);
    unique_ptr<BlockHash> bh(new BlockHash);

    try{
        strcpy(bh->hash, response.string_value().c_str());
    }
    catch(std::exception &e){
        string errstr("Cannot deserialize data ");
        _logger->error(errstr + e.what());
        throw ApplicationException(errstr + e.what());
        return nullptr;
    }

    return bh;
}

unique_ptr<Metadata> CPolkaApi::getMetadata(unique_ptr<GetMetadataParams> params){

}

unique_ptr<RuntimeVersion> CPolkaApi::getRuntimeVersion(unique_ptr<GetRuntimeVersionParams> params){

    Json query = Json::object{
        {"method", "chain_getRuntimeVersion"}, 
        {"params", Json::array{params->blockHash}}
        };

    Json response = _jsonRpc->request(query);
    unique_ptr<RuntimeVersion> rv(new RuntimeVersion);

    try{
    strcpy(rv->specName, response["specName"].string_value().c_str());
    strcpy(rv->implName, response["implName"].string_value().c_str());
    rv->authoritingVersion = response["authoritingVersion"].int_value();
    rv->specVersion = atoi(response["specVersion"].string_value().c_str());
    rv->implVersion = atoi(response["implVersion"].string_value().c_str());
    }
    catch(std::exception &e){
        int i = 0;
        for(Json item: response["apis"].array_items()){
            strcpy(rv->api[i].num, item[0].string_value().c_str());
            rv->api[i].id = item[1].int_value();
            i++;
        }
    }

    return rv;
}