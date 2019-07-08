#include "polkadot.h"

INITIALIZE_EASYLOGGINGPP

void EasyLogger::info(string message) {
    _mtx.lock();
    string outstr = "";
    if (message.length() <= MAX_LOG_MSG_LENGTH)
        outstr = message;
    else
        outstr = message.substr(0, MAX_LOG_MSG_LENGTH) + "...";
    LOG(INFO) << outstr;
    _mtx.unlock();
}
void EasyLogger::error(string message) {
    _mtx.lock();
    LOG(ERROR) << message;
    _mtx.unlock();
}
void EasyLogger::warning(string message) {
    _mtx.lock();
    LOG(WARNING) << message;
    _mtx.unlock();
}
