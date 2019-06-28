#include "polkadot.h"

INITIALIZE_EASYLOGGINGPP

void EasyLogger::info(string message) {
    _mtx.lock();
    LOG(INFO) << message;
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
