#include "polkadot.h"

void LoggerStub::info(string message) { cout << "info: " << message << endl; }
void LoggerStub::error(string message) { cout << "error: " << message << endl; }
void LoggerStub::warning(string message) { cout << "warning: " << message << endl; }
