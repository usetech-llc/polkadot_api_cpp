/*
  response chain_getBlockHash
*/

struct BlockHash {
    char hash[MAX_NAME_SIZE];
};

/*
  params for chain_getBlockHash
*/
struct GetBlockHashParams {
    int blockNumber;
};