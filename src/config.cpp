#include "polkadot.h"

/**
 *  Get node URL from config.json file, if it exists, otherwise return default node
 */
string CConfig::getNodeUrl() {
    string nodeUrl = CConstants::parity_node_url;
    try {
        std::ifstream t(CConstants::config_file);
        std::string jsonConf((std::istreambuf_iterator<char>(t)), std::istreambuf_iterator<char>());

        string err;
        Json conf = Json::parse(jsonConf, err);

        if (conf["nodeUrl"].string_value().length() > 0)
            nodeUrl = conf["nodeUrl"].string_value();
    } catch (...) {
    }

    return nodeUrl;
}
