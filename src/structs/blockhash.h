/*
  response chain_getBlockHash
*/

struct BlockHash {
    char hash[STRING_SIZE];
};

/*
  params for chain_getBlockHash
*/
struct GetBlockHashParams {
    int blockNumber;
};