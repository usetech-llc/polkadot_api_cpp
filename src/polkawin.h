#include <thread>

#define _WINSOCK_DEPRECATED_NO_WARNINGS
#define _CRT_SECURE_NO_WARNINGS
#define strncasecmp _strnicmp
#define strcasecmp _stricmp

typedef uint8_t u_int8_t;
typedef uint16_t u_int16_t;
typedef uint32_t u_int32_t;

void usleep(__int64 usec);
