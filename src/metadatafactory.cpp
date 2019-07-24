#include "polkadot.h"

// class MetadataFactory {
// private:
//     string _buffer;
//     unique_ptr<MDV0> getMetadataV0();
//     unique_ptr<MDV5> getMetadataV5();

// public:
//     Metadata(string inputData);
//     int getVersion();
//     template<class T>
//     unique_ptr<T> getMetadata(int version);
// };

using namespace scale;
//--------------------------- V0
unique_ptr<OuterEventV0> getEventV0(std::string &str) {
    unique_ptr<OuterEventV0> oe(new OuterEventV0);

    int nameLen = decodeCompactInteger(str);
    auto name = extractString(str, nameLen);

    strcpy(oe->name, name.c_str());
    int eventArgCount = decodeCompactInteger(str);

    for (int gi = 0; gi < eventArgCount; gi++) {
        EventArgV0 ea;
        auto eanamestringsize = decodeCompactInteger(str);
        auto eaname = extractString(str, eanamestringsize);
        strcpy(ea.name, eaname.c_str());

        auto argitemsize = decodeCompactInteger(str);
        for (int i = 0; i < argitemsize; i++) {
            auto currentargitemsize = decodeCompactInteger(str);

            ArgItemV0 ai;
            strcpy(ai.value, extractString(str, currentargitemsize).c_str());
            ea.argItem[i] = ai;
        }

        auto documentssize = decodeCompactInteger(str);
        for (int i = 0; i < documentssize; i++) {
            EventDocumentationV0 ed;

            auto currentdocumentssize = decodeCompactInteger(str);
            strcpy(ed.value, extractString(str, currentdocumentssize).c_str());
            ea.eventDoc[i] = ed;
        }

        oe->eventArg[gi] = ea;
    }

    return move(oe);
};

unique_ptr<ModuleV0> getModuleV0(std::string &str) {

    unique_ptr<ModuleV0> module(new ModuleV0);

    int nameLen = decodeCompactInteger(str);
    strcpy(module->prefix, extractString(str, nameLen).c_str());

    int moduleName = decodeCompactInteger(str);
    strcpy(module->module.name, extractString(str, moduleName).c_str());

    int callName = decodeCompactInteger(str);
    strcpy(module->module.call.name, extractString(str, callName).c_str());

    int callFuntionsCount = decodeCompactInteger(str);
    for (int i = 0; i < callFuntionsCount; i++) {
        // get id
        module->module.call.fn1[i].id = nextWord(str);

        int nameLen = decodeCompactInteger(str);
        auto name = extractString(str, nameLen);
        strcpy(module->module.call.fn1[i].name, name.c_str());

        int argsCount = decodeCompactInteger(str);
        for (int ai = 0; ai < argsCount; ai++) {
            int argNameLen = decodeCompactInteger(str);
            strcpy(module->module.call.fn1[i].args[ai].name, extractString(str, argNameLen).c_str());

            int argTypeLen = decodeCompactInteger(str);
            strcpy(module->module.call.fn1[i].args[ai].type, extractString(str, argTypeLen).c_str());
        }

        int docCount = decodeCompactInteger(str);
        for (int di = 0; di < docCount; di++) {
            auto docLen = decodeCompactInteger(str);
            strcpy(module->module.call.fn1[i].documentation[di], extractString(str, docLen).c_str());
        }
    }

    // storage is not null
    auto storageIsset = nextByte(str);
    if (!storageIsset)
        return move(module);

    int storagePrefix = decodeCompactInteger(str);
    strcpy(module->storage.prefix, extractString(str, storagePrefix).c_str());

    int storageFuncsCount = decodeCompactInteger(str);
    for (int i = 0; i < storageFuncsCount; i++) {
        int nameLen = decodeCompactInteger(str);
        auto name = extractString(str, nameLen);
        strcpy(module->storage.function[i].name, name.c_str());

        auto type = nextByte(str);
        auto hasSecondType = nextByte(str);

        int type1Len = decodeCompactInteger(str);
        auto type1 = extractString(str, type1Len);
        string type2;

        if (hasSecondType > 0) {
            int type2Len = decodeCompactInteger(str);
            type2 = extractString(str, type2Len);
        }

        module->storage.function->type->typeName = type;
        // detect type
        if (type == 0) {
            strcpy(module->storage.function[i].type->value, type1.c_str());
        } else {
            strcpy(module->storage.function[i].type->value, type2.c_str());
            strcpy(module->storage.function[i].type->key, type1.c_str());
        }

        // extract fallback as raw hex
        auto fallbackLen = decodeCompactInteger(str);
        auto fallback = str.substr(0, fallbackLen * 2);
        str = str.substr(fallbackLen * 2);
        strcpy(module->storage.function[i].fallback, fallback.c_str());

        // documents count
        auto docCount = decodeCompactInteger(str);
        for (int di = 0; di < docCount; di++) {
            auto docStringLen = decodeCompactInteger(str);
            auto docItem = extractString(str, docStringLen);
            strcpy(module->storage.function[i].documentation[di], docItem.c_str());
        }
    }

    return move(module);
}

DispachCallV0 getOuterDispachV0(std::string &str) {

    // unique_ptr<DispachCallV0> dc(new DispachCallV0);
    DispachCallV0 dc;

    // extract name
    int nameLen = decodeCompactInteger(str);
    strcpy(dc.name, extractString(str, nameLen).c_str());

    // extract prefix
    int prefixLen = decodeCompactInteger(str);
    strcpy(dc.prefix, extractString(str, prefixLen).c_str());

    // extract index
    dc.index = nextWord(str);

    return move(dc);
}

unique_ptr<MDV0> fillV0Metadata(string str) {
    MDV0 *result = new MDV0();
    memset((void *)result, 0, sizeof(MDV0));
    unique_ptr<MDV0> md(result);

    md->oew = unique_ptr<OuterEventWrapperV0>(new OuterEventWrapperV0);
    md->dispach = unique_ptr<OuterDispachV0>(new OuterDispachV0);

    // ---------- Events
    int len = decodeCompactInteger(str);
    strcpy(md->oew->name, extractString(str, len).c_str());

    // get items count
    int eventsLen = decodeCompactInteger(str);
    for (int i = 0; i < eventsLen; i++) {
        md->oew->eventArg[i] = getEventV0(str);
    }

    // ---------- Modules
    int modulesLen = decodeCompactInteger(str);
    for (int i = 0; i < modulesLen; i++) {
        md->module[i] = getModuleV0(str);
    }

    //----------- OuterDispach
    auto odnLen = decodeCompactInteger(str);
    strcpy(md->dispach->name, extractString(str, odnLen).c_str());

    int outerDispachLen = decodeCompactInteger(str);
    for (int i = 0; i < outerDispachLen; i++) {
        md->dispach->call[i] = move(getOuterDispachV0(str));
    }

    return move(md);
};

//--------------------------- V4

unique_ptr<CallV4> getCallV4(std::string &str) {

    unique_ptr<CallV4> call(new CallV4);

    int callNameLen = decodeCompactInteger(str);
    strcpy(call->name, extractString(str, callNameLen).c_str());

    // args count
    int args = decodeCompactInteger(str);
    for (int i = 0; i < args; i++) {
        FunctionCallArgV4 arg;

        int argNameLen = decodeCompactInteger(str);
        strcpy(arg.name, extractString(str, argNameLen).c_str());

        int argTypeLen = decodeCompactInteger(str);
        strcpy(arg.type, extractString(str, argTypeLen).c_str());
        call->args[i] = move(arg);
    }

    // documents count
    auto docCount = decodeCompactInteger(str);
    for (int di = 0; di < docCount; di++) {
        auto docStringLen = decodeCompactInteger(str);
        auto docItem = extractString(str, docStringLen);
        strcpy(call->documentation[di], docItem.c_str());
    }

    return move(call);
};

unique_ptr<EventArgV4> getEventV4(std::string &str) {

    unique_ptr<EventArgV4> ea(new EventArgV4);

    int callNameLen = decodeCompactInteger(str);
    strcpy(ea->name, extractString(str, callNameLen).c_str());

    // args count
    int args = decodeCompactInteger(str);
    for (int i = 0; i < args; i++) {
        int argLen = decodeCompactInteger(str);
        strcpy(ea->args[i], extractString(str, argLen).c_str());
    }

    // documents count
    auto docCount = decodeCompactInteger(str);
    for (int di = 0; di < docCount; di++) {
        auto docStringLen = decodeCompactInteger(str);
        auto docItem = extractString(str, docStringLen);
        strcpy(ea->documentation[di], docItem.c_str());
    }

    return move(ea);
}

//--------------------------- V5

unique_ptr<StorageV5> getStorageV5(std::string &str) {

    unique_ptr<StorageV5> storage(new StorageV5);

    int storageNameLen = decodeCompactInteger(str);
    strcpy(storage->name, extractString(str, storageNameLen).c_str());

    storage->modifier = nextByte(str);
    auto hasSecondType = nextByte(str);

    storage->type.type = hasSecondType != 0 ? nextByte(str) : 0;

    int type1Len = decodeCompactInteger(str);
    auto type1 = extractString(str, type1Len);
    strcpy(storage->type.key1, type1.c_str());

    // map
    if (hasSecondType == 1) {
        // get value
        int valLen = decodeCompactInteger(str);
        auto value = extractString(str, valLen);
        strcpy(storage->type.value, value.c_str());
    }

    // double map
    if (hasSecondType == 2) {
        // get second key
        int type2Len = decodeCompactInteger(str);
        auto type2 = extractString(str, type2Len);
        strcpy(storage->type.value, type2.c_str());

        // get value
        int valLen = decodeCompactInteger(str);
        auto value = extractString(str, valLen);
        strcpy(storage->type.value, value.c_str());
    }

    if (hasSecondType != 0) {
        storage->type.isLinked = nextByte(str);
    }

    // extract fallback as raw hex
    auto fallbackLen = decodeCompactInteger(str);
    auto fallback = str.substr(0, fallbackLen * 2);
    str = str.substr(fallbackLen * 2);
    strcpy(storage->fallback, fallback.c_str());

    // documents count
    auto docCount = decodeCompactInteger(str);
    for (int di = 0; di < docCount; di++) {
        auto docStringLen = decodeCompactInteger(str);
        auto docItem = extractString(str, docStringLen);
        strcpy(storage->documentation[di], docItem.c_str());
    }

    return move(storage);
}

unique_ptr<CallV5> getCallV5(std::string &str) {

    unique_ptr<CallV5> call(new CallV5);

    int callNameLen = decodeCompactInteger(str);
    strcpy(call->name, extractString(str, callNameLen).c_str());

    // args count
    int args = decodeCompactInteger(str);
    for (int i = 0; i < args; i++) {
        FunctionCallArgV5 arg;

        int argNameLen = decodeCompactInteger(str);
        strcpy(arg.name, extractString(str, argNameLen).c_str());

        int argTypeLen = decodeCompactInteger(str);
        strcpy(arg.type, extractString(str, argTypeLen).c_str());
        call->args[i] = move(arg);
    }

    // documents count
    auto docCount = decodeCompactInteger(str);
    for (int di = 0; di < docCount; di++) {
        auto docStringLen = decodeCompactInteger(str);
        auto docItem = extractString(str, docStringLen);
        strcpy(call->documentation[di], docItem.c_str());
    }

    return move(call);
};

unique_ptr<EventArgV5> getEventV5(std::string &str) {

    unique_ptr<EventArgV5> ea(new EventArgV5);

    int callNameLen = decodeCompactInteger(str);
    strcpy(ea->name, extractString(str, callNameLen).c_str());

    // args count
    int args = decodeCompactInteger(str);
    for (int i = 0; i < args; i++) {
        int argLen = decodeCompactInteger(str);
        strcpy(ea->args[i], extractString(str, argLen).c_str());
    }

    // documents count
    auto docCount = decodeCompactInteger(str);
    for (int di = 0; di < docCount; di++) {
        auto docStringLen = decodeCompactInteger(str);
        auto docItem = extractString(str, docStringLen);
        strcpy(ea->documentation[di], docItem.c_str());
    }

    return move(ea);
}

//--------------------------- V6

unique_ptr<StorageV6> getStorageV6(std::string &str) {

    unique_ptr<StorageV6> storage(new StorageV6);

    int storageNameLen = decodeCompactInteger(str);
    strcpy(storage->name, extractString(str, storageNameLen).c_str());

    storage->modifier = nextByte(str);
    auto hasSecondType = nextByte(str);

    storage->type.type = hasSecondType != 0 ? nextByte(str) : 0;

    int type1Len = decodeCompactInteger(str);
    auto type1 = extractString(str, type1Len);
    strcpy(storage->type.key1, type1.c_str());

    // map
    if (hasSecondType == 1) {
        // get value
        int valLen = decodeCompactInteger(str);
        auto value = extractString(str, valLen);
        strcpy(storage->type.value, value.c_str());
    }

    // double map
    if (hasSecondType == 2) {
        // get second key
        int type2Len = decodeCompactInteger(str);
        auto type2 = extractString(str, type2Len);
        strcpy(storage->type.value, type2.c_str());

        // get value
        int valLen = decodeCompactInteger(str);
        auto value = extractString(str, valLen);
        strcpy(storage->type.value, value.c_str());
    }

    if (hasSecondType != 0) {
        storage->type.isLinked = nextByte(str);
    }

    // extract fallback as raw hex
    auto fallbackLen = decodeCompactInteger(str);
    auto fallback = str.substr(0, fallbackLen * 2);
    str = str.substr(fallbackLen * 2);
    strcpy(storage->fallback, fallback.c_str());

    // documents count
    auto docCount = decodeCompactInteger(str);
    for (int di = 0; di < docCount; di++) {
        auto docStringLen = decodeCompactInteger(str);
        auto docItem = extractString(str, docStringLen);
        strcpy(storage->documentation[di], docItem.c_str());
    }

    return move(storage);
}

unique_ptr<CallV6> getCallV6(std::string &str) {

    unique_ptr<CallV6> call(new CallV6);

    int callNameLen = decodeCompactInteger(str);
    strcpy(call->name, extractString(str, callNameLen).c_str());

    // args count
    int args = decodeCompactInteger(str);
    for (int i = 0; i < args; i++) {
        FunctionCallArgV6 arg;

        int argNameLen = decodeCompactInteger(str);
        strcpy(arg.name, extractString(str, argNameLen).c_str());

        int argTypeLen = decodeCompactInteger(str);
        strcpy(arg.type, extractString(str, argTypeLen).c_str());
        call->args[i] = move(arg);
    }

    // documents count
    auto docCount = decodeCompactInteger(str);
    for (int di = 0; di < docCount; di++) {
        auto docStringLen = decodeCompactInteger(str);
        auto docItem = extractString(str, docStringLen);
        strcpy(call->documentation[di], docItem.c_str());
    }

    return move(call);
};

unique_ptr<EventArgV6> getEventV6(std::string &str) {

    unique_ptr<EventArgV6> ea(new EventArgV6);

    int callNameLen = decodeCompactInteger(str);
    strcpy(ea->name, extractString(str, callNameLen).c_str());

    // args count
    int args = decodeCompactInteger(str);
    for (int i = 0; i < args; i++) {
        int argLen = decodeCompactInteger(str);
        strcpy(ea->args[i], extractString(str, argLen).c_str());
    }

    // documents count
    auto docCount = decodeCompactInteger(str);
    for (int di = 0; di < docCount; di++) {
        auto docStringLen = decodeCompactInteger(str);
        auto docItem = extractString(str, docStringLen);
        strcpy(ea->documentation[di], docItem.c_str());
    }

    return move(ea);
}

ConstV6 getConstsV6(std::string &str) {

    ConstV6 cons;

    // extract name
    int nameLen = decodeCompactInteger(str);
    strcpy(cons.name, extractString(str, nameLen).c_str());

    // extract type
    int typeLen = decodeCompactInteger(str);
    strcpy(cons.type, extractString(str, typeLen).c_str());

    //// extract value
    int valueLen = decodeCompactInteger(str);
    auto value = str.substr(0, valueLen * 2);
    str = str.substr(valueLen * 2);
    strcpy(cons.value, value.c_str());

    //// documents count
    auto docCount = decodeCompactInteger(str);
    for (int di = 0; di < docCount; di++) {
        auto docStringLen = decodeCompactInteger(str);
        auto docItem = extractString(str, docStringLen);
        strcpy(cons.documentation[di], docItem.c_str());
    }

    return move(cons);
};

unique_ptr<MDV5> fillV5Metadata(std::string str) {
    // skip 5 magic bytes
    nextByte(str);
    nextByte(str);
    nextByte(str);
    nextByte(str);
    nextByte(str);

    MDV5 *result = new MDV5();
    memset((void *)result, 0, sizeof(MDV5));
    unique_ptr<MDV5> md(result);
    int mLen = decodeCompactInteger(str);
    for (auto moduleIndex = 0; moduleIndex < mLen; moduleIndex++) {
        // create module instance
        unique_ptr<ModuleV5> module(new ModuleV5);
        md->module[moduleIndex] = move(module);

        // get module name
        int moduleNameLen = decodeCompactInteger(str);
        strcpy(md->module[moduleIndex]->name, extractString(str, moduleNameLen).c_str());

        // get module prefix
        int modulePrefixLen = decodeCompactInteger(str);
        strcpy(md->module[moduleIndex]->prefix, extractString(str, modulePrefixLen).c_str());

        // ---------- Storage
        // storage is not null
        auto storageIsset = nextByte(str);
        if (storageIsset != 0) {
            int storageLen = decodeCompactInteger(str);
            if (storageLen == 0){
                unique_ptr<StorageV5> storage(new StorageV5);
                md->module[moduleIndex]->storage[0] = move(storage);
            }

            for (int i = 0; i < storageLen; i++) {
                md->module[moduleIndex]->storage[i] = getStorageV5(str);
            }
        } else {
            for (int i = 0; i < COLLECTION_SIZE; i++) {
                md->module[moduleIndex]->storage[i] = nullptr;
            }
        }

        // ---------- Calls
        // calls is not null
        auto callsIsset = nextByte(str);
        if (callsIsset != 0) {
            int callsCount = decodeCompactInteger(str);
            if (callsCount == 0){
                unique_ptr<CallV5> call(new CallV5);
                md->module[moduleIndex]->call[0] = move(call);
            }
            for (int i = 0; i < callsCount; i++) {
                md->module[moduleIndex]->call[i] = getCallV5(str);
            }
        } else {
            for (int i = 0; i < COLLECTION_SIZE; i++) {
                md->module[moduleIndex]->call[i] = nullptr;
            }
        }

        // ---------- Events
        // events is not null
        auto eventsIsset = nextByte(str);
        if (eventsIsset != 0) {
            int eventsCount = decodeCompactInteger(str);
            if (eventsCount == 0){
                unique_ptr<EventArgV5> ev(new EventArgV5);
                md->module[moduleIndex]->ev[0] = move(ev);
            }
            for (int i = 0; i < eventsCount; i++) {
                md->module[moduleIndex]->ev[i] = getEventV5(str);
            }
        } else {
            for (int i = 0; i < COLLECTION_SIZE; i++) {
                md->module[moduleIndex]->ev[i] = nullptr;
            }
        }
    }

    return move(md);
};

unique_ptr<MDV6> fillV6Metadata(std::string str) {
    // magic bytes
    auto magic1 = nextByte(str);
    auto magic2 = nextByte(str);
    auto magic3 = nextByte(str);
    auto magic4 = nextByte(str);
    auto magic5 = nextByte(str);

    unique_ptr<MDV6> md(new MDV6);
    int mLen = decodeCompactInteger(str);
    for (auto moduleIndex = 0; moduleIndex < mLen; moduleIndex++) {
        // create module instance
        unique_ptr<ModuleV6> module(new ModuleV6);
        md->module[moduleIndex] = move(module);

        // get module name
        int moduleNameLen = decodeCompactInteger(str);
        strcpy(md->module[moduleIndex]->name, extractString(str, moduleNameLen).c_str());

        // get module prefix
        int modulePrefixLen = decodeCompactInteger(str);
        strcpy(md->module[moduleIndex]->prefix, extractString(str, modulePrefixLen).c_str());

        // ---------- Storage
        // storage is not null
        auto storageIsset = nextByte(str);
        if (storageIsset != 0) {
            int storageLen = decodeCompactInteger(str);
            if (storageLen == 0){
                unique_ptr<StorageV6> storage(new StorageV6);
                md->module[moduleIndex]->storage[0] = move(storage);
            }
            for (int i = 0; i < storageLen; i++) {
                md->module[moduleIndex]->storage[i] = getStorageV6(str);
            }
        } else {
            for (int i = 0; i < COLLECTION_SIZE; i++) {
                md->module[moduleIndex]->storage[i] = nullptr;
            }
        }

        // ---------- Calls
        // calls is not null
        auto callsIsset = nextByte(str);
        if (callsIsset != 0) {
            int callsCount = decodeCompactInteger(str);
            if (callsCount == 0){
                unique_ptr<CallV6> call(new CallV6);
                md->module[moduleIndex]->call[0] = move(call);
            }

            for (int i = 0; i < callsCount; i++) {
                md->module[moduleIndex]->call[i] = getCallV6(str);
            }
        } else {
            for (int i = 0; i < COLLECTION_SIZE; i++) {
                md->module[moduleIndex]->call[i] = nullptr;
            }
        }

        // ---------- Events
        // events is not null
        auto eventsIsset = nextByte(str);
        if (eventsIsset != 0) {
            int eventsCount = decodeCompactInteger(str);
            if (eventsCount == 0){
                unique_ptr<EventArgV6> ev(new EventArgV6);
                md->module[moduleIndex]->ev[0] = move(ev);
            }

            for (int i = 0; i < eventsCount; i++) {
                md->module[moduleIndex]->ev[i] = getEventV6(str);
            }
        } else {
            for (int i = 0; i < COLLECTION_SIZE; i++) {
                md->module[moduleIndex]->ev[i] = nullptr;
            }
        }

        // ---------- Consts
        // consts doesn`t have isset flag, straight count number instead of that
        auto constsCount = decodeCompactInteger(str);
        if (constsCount != 0) {
            for (int i = 0; i < constsCount; i++) {
                md->module[moduleIndex]->cons[i] = getConstsV6(str);
            }
        }
    }

    return move(md);
};

unique_ptr<StorageV4> getStorageV4(std::string &str) {

    unique_ptr<StorageV4> storage(new StorageV4);

    int storageNameLen = decodeCompactInteger(str);
    strcpy(storage->name, extractString(str, storageNameLen).c_str());

    storage->modifier = nextByte(str);
    storage->type.type = nextByte(str);

    int type1Len = decodeCompactInteger(str);
    auto type1 = extractString(str, type1Len);
    strcpy(storage->type.key1, type1.c_str());

    if (storage->type.type != 0) {
        int secondTypeLen = decodeCompactInteger(str);
        strcpy(storage->type.key2, extractString(str, secondTypeLen).c_str());
        storage->type.hasher = nextByte(str);
    }

    // extract fallback as raw hex
    auto fallbackLen = decodeCompactInteger(str);
    auto fallback = str.substr(0, fallbackLen * 2);
    str = str.substr(fallbackLen * 2);
    strcpy(storage->fallback, fallback.c_str());

    // documents count
    auto docCount = decodeCompactInteger(str);
    for (int di = 0; di < docCount; di++) {
        auto docStringLen = decodeCompactInteger(str);
        auto docItem = extractString(str, docStringLen);
        strcpy(storage->documentation[di], docItem.c_str());
    }

    return move(storage);
}

unique_ptr<MDV4> fillV4Metadata(std::string str) {
    // magic bytes
    auto magic1 = nextByte(str);
    auto magic2 = nextByte(str);
    auto magic3 = nextByte(str);
    auto magic4 = nextByte(str);
    auto magic5 = nextByte(str);

    unique_ptr<MDV4> md(new MDV4);
    int mLen = decodeCompactInteger(str);
    for (auto moduleIndex = 0; moduleIndex < mLen; moduleIndex++) {
        // create module instance
        unique_ptr<ModuleV4> module(new ModuleV4);
        md->module[moduleIndex] = move(module);

        // get module name
        int moduleNameLen = decodeCompactInteger(str);
        strcpy(md->module[moduleIndex]->name, extractString(str, moduleNameLen).c_str());

        // get module prefix
        int modulePrefixLen = decodeCompactInteger(str);
        strcpy(md->module[moduleIndex]->prefix, extractString(str, modulePrefixLen).c_str());

        // ---------- Storage
        // storage is not null
        auto storageIsset = nextByte(str);
        if (storageIsset != 0) {
            int storageLen = decodeCompactInteger(str);
            if (storageLen == 0){
                unique_ptr<StorageV4> storage(new StorageV4);
                md->module[moduleIndex]->storage[0] = move(storage);
            }

            for (int i = 0; i < storageLen; i++) {
                md->module[moduleIndex]->storage[i] = getStorageV4(str);
            }
        } else {
            for (int i = 0; i < COLLECTION_SIZE; i++) {
                md->module[moduleIndex]->storage[i] = nullptr;
            }
        }

        // ---------- Calls
        // calls is not null
        auto callsIsset = nextByte(str);
        if (callsIsset != 0) {
            int callsCount = decodeCompactInteger(str);
            if (callsCount == 0){
                unique_ptr<CallV4> call(new CallV4);
                md->module[moduleIndex]->call[0] = move(call);
            }

            for (int i = 0; i < callsCount; i++) {
                md->module[moduleIndex]->call[i] = getCallV4(str);
            }
        } else {
            for (int i = 0; i < COLLECTION_SIZE; i++) {
                md->module[moduleIndex]->call[i] = nullptr;
            }
        }

        // ---------- Events
        // events is not null
        auto eventsIsset = nextByte(str);
        if (eventsIsset != 0) {
            int eventsCount = decodeCompactInteger(str);
            if (eventsCount == 0){
                unique_ptr<EventArgV4> ev(new EventArgV4);
                md->module[moduleIndex]->ev[0] = move(ev);
            }

            for (int i = 0; i < eventsCount; i++) {
                md->module[moduleIndex]->ev[i] = getEventV4(str);
            }
        } else {
            for (int i = 0; i < COLLECTION_SIZE; i++) {
                md->module[moduleIndex]->ev[i] = nullptr;
            }
        }
    }

    return move(md);
};

MetadataFactory::MetadataFactory(ILogger *logger) {
    _logger = logger;
    _version = -1;
}

void MetadataFactory::setInputData(string hexString) {
    _buffer = hexString;

    // ofstream out("lastMetadata.txt");
    // out << hexString;
    // out.close();
}

unique_ptr<MDV0> MetadataFactory::getMetadataV0() {

    unique_ptr<MDV0> result;
    try {
        result = move(fillV0Metadata(_buffer.substr()));

        if (result != nullptr) {
            _version = 0;
            _logger->info("Metadata version V0 detected");
        }
    } catch (...) {
        _logger->info("Metadata version is not V0");
    }

    return result;
}

unique_ptr<MDV4> MetadataFactory::getMetadataV4() {

    unique_ptr<MDV4> result;
    try {
        result = move(fillV4Metadata(_buffer.substr()));

        if (result != nullptr) {
            _version = 4;
            _logger->info("Metadata version V4 detected");
        }
    } catch (...) {
        _logger->info("Metadata version is not V4");
    }

    return result;
}

unique_ptr<MDV5> MetadataFactory::getMetadataV5() {

    unique_ptr<MDV5> result;
    try {
        result = move(fillV5Metadata(_buffer.substr()));

        if (result != nullptr) {
            _version = 5;
            _logger->info("Metadata version V5 detected");
        }
    } catch (...) {
        _logger->info("Metadata version is not V5");
    }

    return result;
}

unique_ptr<MDV6> MetadataFactory::getMetadataV6() {

    unique_ptr<MDV6> result;
    try {
        result = move(fillV6Metadata(_buffer.substr()));

        if (result != nullptr) {
            _version = 6;
            _logger->info("Metadata version V6 detected");
        }
    } catch (...) {
        _logger->info("Metadata version is not V6");
    }

    return result;
}

int MetadataFactory::getVersion() {
    getMetadataV0();

    if (_version == -1)
        getMetadataV5();

    if (_version == -1)
        getMetadataV6();

    return _version;
}
