
struct FunctionCallArgV4 {
    char name[STRING_SIZE];
    char type[STRING_SIZE];
};

struct EventArgV4 {
    char name[STRING_SIZE];
    char args[COLLECTION_SIZE][STRING_SIZE];
    char documentation[COLLECTION_SIZE][STRING_SIZE];
};

struct CallV4 {
    char name[STRING_SIZE];
    FunctionCallArgV4 args[STRING_SIZE];
    char documentation[COLLECTION_SIZE][STRING_SIZE];
};

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
    unique_ptr<StorageV4> storage[COLLECTION_SIZE];
    unique_ptr<CallV4> call[COLLECTION_SIZE];
    unique_ptr<EventArgV4> ev[COLLECTION_SIZE];
};

struct MDV4 {
    unique_ptr<ModuleV4> module[COLLECTION_SIZE];
};
