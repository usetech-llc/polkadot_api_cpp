#include "polkadot.h"

INITIALIZE_EASYLOGGINGPP

void EasyLogger::info(string message) { LOG(INFO) << message; }
void EasyLogger::error(string message) { LOG(ERROR) << message; }
void EasyLogger::warning(string message) { LOG(WARNING) << message; }
