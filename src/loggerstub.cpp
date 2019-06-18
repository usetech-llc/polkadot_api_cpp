#include "polkadot.h"

void LoggerStub::info(string message) { cout << "info: " << message; }
void LoggerStub::error(string message) { cout << "error: " << message; }
void LoggerStub::warning(string message) { cout << "warning: " << message; }