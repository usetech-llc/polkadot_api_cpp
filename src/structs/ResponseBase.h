/*
  basic response data
*/
#define DEFAULT_SIZE 256
#define MAX_NAME_SIZE 64

struct ResponseBase {
    char jsonrpc[MAX_NAME_SIZE];
    int id;
}
