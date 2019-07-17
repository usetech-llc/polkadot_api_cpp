
struct ConstV6 {
    char name[STRING_SIZE];
    char type[STRING_SIZE];
    char value[STRING_SIZE];
    char documentation[COLLECTION_SIZE][STRING_SIZE];
};

struct ModuleV6 {
    char name[STRING_SIZE];
    char prefix[STRING_SIZE];
    StorageV5 storage[COLLECTION_SIZE];
    CallV5 call[COLLECTION_SIZE];
    EventArgV5 ev[COLLECTION_SIZE];
    ConstV6 cons[COLLECTION_SIZE];
};

struct MDV6 {
    unique_ptr<ModuleV6> module[COLLECTION_SIZE];
};