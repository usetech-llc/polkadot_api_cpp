
struct FunctionCallArgV5 {
    char name[STRING_SIZE];
    char type[STRING_SIZE];
};

struct EventArgV5 {
    char name[STRING_SIZE];
    char args[COLLECTION_SIZE][STRING_SIZE];
    char documentation[COLLECTION_SIZE][STRING_SIZE];
};

struct CallV5 {
    char name[STRING_SIZE];
    FunctionCallArgV5 args[STRING_SIZE];
    char documentation[COLLECTION_SIZE][STRING_SIZE];
};

struct FuncTypeV5 {
    // 0 - plain, 1 - map, 2 - doublemap
    unsigned int type;
    char hasher[STRING_SIZE];
    char key1[STRING_SIZE];
    char key2[STRING_SIZE];
    char value[STRING_SIZE];
    char key2hasher[STRING_SIZE];
    bool isLinked;
};

struct StorageV5 {
    char name[STRING_SIZE];
    // 0 - Optional, 1 - Default
    unsigned char modifier;
    FuncTypeV5 type;
    char fallback[STRING_SIZE];
    char documentation[COLLECTION_SIZE][STRING_SIZE];
};

struct ModuleV5 {
    char name[STRING_SIZE];
    char prefix[STRING_SIZE];
    StorageV5 storage[COLLECTION_SIZE];
    CallV5 call[COLLECTION_SIZE];
    EventArgV5 ev[COLLECTION_SIZE];
};

struct MDV5 {
    unique_ptr<ModuleV5> module[COLLECTION_SIZE];
};