struct FuncTypeV4 {
    // 0 - plain, 1 - map
    unsigned int type;
    unsigned int hasher;
    char key1[STRING_SIZE];
    char key2[STRING_SIZE];
};

struct StorageV4 {
    char name[STRING_SIZE];
    // 0 - Optional, 1 - Default
    unsigned char modifier;
    FuncTypeV4 type;
    char fallback[STRING_SIZE];
    char documentation[COLLECTION_SIZE][STRING_SIZE];
};

struct ModuleV4 {
    char name[STRING_SIZE];
    char prefix[STRING_SIZE];
    StorageV4 storage[COLLECTION_SIZE];
    CallV5 call[COLLECTION_SIZE];
    EventArgV5 ev[COLLECTION_SIZE];
};

struct MDV4 {
    unique_ptr<ModuleV4> module[COLLECTION_SIZE];
};
