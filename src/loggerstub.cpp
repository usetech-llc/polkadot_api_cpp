#include "polkadot.h"

void LoggerStub::info(string message) {
    _mtx.lock();
    cout << "info: " << message << endl;
    _mtx.unlock();
}
void LoggerStub::error(string message) {
    _mtx.lock();
    cout << "error: " << message << endl;
    _mtx.unlock();
}
void LoggerStub::warning(string message) {
    _mtx.lock();
    cout << "warning: " << message << endl;
    _mtx.unlock();
}
