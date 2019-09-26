struct EventArgV7 {
    char name[STRING_SIZE];
    char args[COLLECTION_SIZE][STRING_SIZE];
    char documentation[COLLECTION_SIZE][STRING_SIZE];
};

struct FunctionCallArgV7 {
    char name[STRING_SIZE];
    char type[STRING_SIZE];
};

struct CallV7 {
    char name[STRING_SIZE];
    FunctionCallArgV7 args[STRING_SIZE];
    char documentation[COLLECTION_SIZE][STRING_SIZE];
};

struct FuncTypeV7 {
    // 0 - plain, 1 - map, 2 - doublemap
    unsigned int type;
    char hasher[STRING_SIZE];
    char key1[STRING_SIZE];
    char key2[STRING_SIZE];
    char value[STRING_SIZE];
    char key2hasher[STRING_SIZE];
    bool isLinked;
};

struct StorageV7 {
    char name[STRING_SIZE];
    char prefix[STRING_SIZE];
    int modifier;
    FuncTypeV7 type;
    char fallback[STRING_SIZE];
    char documentation[COLLECTION_SIZE][STRING_SIZE];
};

struct StorageCollectionV7 {
    char prefix[STRING_SIZE];
    unique_ptr<StorageV7> items[COLLECTION_SIZE];
};

struct ConstV7 {
    char name[STRING_SIZE];
    char type[STRING_SIZE];
    char value[STRING_SIZE];
    char documentation[COLLECTION_SIZE][STRING_SIZE];
};

struct ModuleV7 {
    char name[STRING_SIZE];
    unique_ptr<StorageCollectionV7> storage;
    unique_ptr<CallV7> call[COLLECTION_SIZE];
    unique_ptr<EventArgV7> ev[COLLECTION_SIZE];
    ConstV7 cons[COLLECTION_SIZE];
};

struct MDV7 {
    unique_ptr<ModuleV7> module[COLLECTION_SIZE];
};

unique_ptr<StorageV7> getStorageV7(std::string &str);
unique_ptr<CallV7> getCallV7(std::string &str);
unique_ptr<EventArgV7> getEventV7(std::string &str);
ConstV7 getConstsV7(std::string &str);
