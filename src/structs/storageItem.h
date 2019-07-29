/*
  Item in state_queryStorage response
*/

typedef struct StorageItem {
    char key[STRING_SIZE];
    char blockHash[STRING_SIZE];
    char value[STRING_SIZE];
} StorageItem;
