#ifdef _WIN32
#include "polkadot.h"

void usleep(__int64 usec) {

	std::this_thread::sleep_for(std::chrono::microseconds(usec));
}
#endif