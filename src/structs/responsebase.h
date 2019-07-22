/*
  basic response data
*/

#define STRING_SIZE 256
#define MAX_PEER_COUNT 256
#define COLLECTION_SIZE 64

struct ResponseBase {
    char jsonrpc[STRING_SIZE];
    int id;
};
