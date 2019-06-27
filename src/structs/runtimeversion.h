/*
  response chain_getRuntimeVersion
*/

struct ApiItem {
    char num[STRING_SIZE];
    int id;
};

struct RuntimeVersion {
    ApiItem api[COLLECTION_SIZE];
    unsigned int authoringVersion;
    char implName[STRING_SIZE];
    int implVersion;
    char specName[STRING_SIZE];
    int specVersion;
};

/*
  params for chain_getRuntimeVersion
*/

struct GetRuntimeVersionParams {
    char blockHash[STRING_SIZE];
};
