

struct FunctionCallArgV6 {
    char name[STRING_SIZE];
    char type[STRING_SIZE];
};

struct EventArgV6 {
    char name[STRING_SIZE];
    char args[COLLECTION_SIZE][STRING_SIZE];
    char documentation[COLLECTION_SIZE][STRING_SIZE];
};

struct CallV6 {
    char name[STRING_SIZE];
    FunctionCallArgV6 args[STRING_SIZE];
    char documentation[COLLECTION_SIZE][STRING_SIZE];
};

struct FuncTypeV6 {
    // 0 - plain, 1 - map, 2 - doublemap
    unsigned int type;
    char hasher[STRING_SIZE];
    char key1[STRING_SIZE];
    char key2[STRING_SIZE];
    char value[STRING_SIZE];
    char key2hasher[STRING_SIZE];
    bool isLinked;
};

struct StorageV6 {
    char name[STRING_SIZE];
    // 0 - Optional, 1 - Default
    unsigned char modifier;
    FuncTypeV6 type;
    char fallback[STRING_SIZE];
    char documentation[COLLECTION_SIZE][STRING_SIZE];
};

struct ConstV6 {
    char name[STRING_SIZE];
    char type[STRING_SIZE];
    char value[STRING_SIZE];
    char documentation[COLLECTION_SIZE][STRING_SIZE];
};

struct ModuleV6 {
    char name[STRING_SIZE];
    char prefix[STRING_SIZE];
    unique_ptr<StorageV6> storage[COLLECTION_SIZE];
    unique_ptr<CallV6> call[COLLECTION_SIZE];
    unique_ptr<EventArgV6> ev[COLLECTION_SIZE];
    ConstV6 cons[COLLECTION_SIZE];
};

struct MDV6 {
    unique_ptr<ModuleV6> module[COLLECTION_SIZE];
};