#include "../polkacpp.h"

unique_ptr<StorageV7> getStorageV7(std::string &str) {

    unique_ptr<StorageV7> storage(new StorageV7);

    int storageNameLen = scale::decodeCompactInteger(str);
    strcpy(storage->name, scale::extractString(str, storageNameLen).c_str());

    storage->modifier = scale::nextByte(str);
    auto hasSecondType = scale::nextByte(str);

    storage->type.type = hasSecondType != 0 ? scale::nextByte(str) : 0;

    int type1Len = scale::decodeCompactInteger(str);
    auto type1 = scale::extractString(str, type1Len);
    strcpy(storage->type.key1, type1.c_str());

    // map
    if (hasSecondType == 1) {
        // get value
        int valLen = scale::decodeCompactInteger(str);
        auto value = scale::extractString(str, valLen);
        strcpy(storage->type.value, value.c_str());
    }

    // double map
    if (hasSecondType == 2) {
        // get second key
        int type2Len = scale::decodeCompactInteger(str);
        auto type2 = scale::extractString(str, type2Len);
        strcpy(storage->type.value, type2.c_str());

        // get value
        int valLen = scale::decodeCompactInteger(str);
        auto value = scale::extractString(str, valLen);
        strcpy(storage->type.value, value.c_str());
    }

    if (hasSecondType != 0) {
        storage->type.isLinked = scale::nextByte(str);
    }

    // extract fallback as raw hex
    auto fallbackLen = scale::decodeCompactInteger(str);
    auto fallback = str.substr(0, fallbackLen * 2);
    str = str.substr(fallbackLen * 2);
    strcpy(storage->fallback, fallback.c_str());

    // documents count
    auto docCount = scale::decodeCompactInteger(str);
    for (int di = 0; di < docCount; di++) {
        auto docStringLen = scale::decodeCompactInteger(str);
        auto docItem = scale::extractString(str, docStringLen);
        strcpy(storage->documentation[di], docItem.c_str());
    }

    return move(storage);
}

unique_ptr<CallV7> getCallV7(std::string &str) {

    unique_ptr<CallV7> call(new CallV7);

    int callNameLen = scale::decodeCompactInteger(str);
    strcpy(call->name, scale::extractString(str, callNameLen).c_str());

    // args count
    int args = scale::decodeCompactInteger(str);
    for (int i = 0; i < args; i++) {
        FunctionCallArgV7 arg;

        int argNameLen = scale::decodeCompactInteger(str);
        strcpy(arg.name, scale::extractString(str, argNameLen).c_str());

        int argTypeLen = scale::decodeCompactInteger(str);
        strcpy(arg.type, scale::extractString(str, argTypeLen).c_str());
        call->args[i] = move(arg);
    }

    // documents count
    auto docCount = scale::decodeCompactInteger(str);
    for (int di = 0; di < docCount; di++) {
        auto docStringLen = scale::decodeCompactInteger(str);
        auto docItem = scale::extractString(str, docStringLen);
        strcpy(call->documentation[di], docItem.c_str());
    }

    return move(call);
};

unique_ptr<EventArgV7> getEventV7(std::string &str) {

    unique_ptr<EventArgV7> ea(new EventArgV7);

    int callNameLen = scale::decodeCompactInteger(str);
    strcpy(ea->name, scale::extractString(str, callNameLen).c_str());

    // args count
    int args = scale::decodeCompactInteger(str);
    for (int i = 0; i < args; i++) {
        int argLen = scale::decodeCompactInteger(str);
        strcpy(ea->args[i], scale::extractString(str, argLen).c_str());
    }

    // documents count
    auto docCount = scale::decodeCompactInteger(str);
    for (int di = 0; di < docCount; di++) {
        auto docStringLen = scale::decodeCompactInteger(str);
        auto docItem = scale::extractString(str, docStringLen);
        strcpy(ea->documentation[di], docItem.c_str());
    }

    return move(ea);
}

ConstV7 getConstsV7(std::string &str) {

    ConstV7 cons;

    // extract name
    int nameLen = scale::decodeCompactInteger(str);
    strcpy(cons.name, scale::extractString(str, nameLen).c_str());

    // extract type
    int typeLen = scale::decodeCompactInteger(str);
    strcpy(cons.type, scale::extractString(str, typeLen).c_str());

    //// extract value
    int valueLen = scale::decodeCompactInteger(str);
    auto value = str.substr(0, valueLen * 2);
    str = str.substr(valueLen * 2);
    strcpy(cons.value, value.c_str());

    //// documents count
    auto docCount = scale::decodeCompactInteger(str);
    for (int di = 0; di < docCount; di++) {
        auto docStringLen = scale::decodeCompactInteger(str);
        auto docItem = scale::extractString(str, docStringLen);
        strcpy(cons.documentation[di], docItem.c_str());
    }

    return move(cons);
};
