#ifdef _WIN32
#include "polkawin.h"
#include <io.h>
#else
#include <thread>
#include <unistd.h>
#endif

// clang-format off
#include <curl/curl.h>
#include <functional>
#include <iostream>
#include <string>
#include <stdbool.h>

// Websockets
#include <websocketpp/client.hpp>
#include <websocketpp/config/asio_client.hpp>
#include <websocketpp/config/asio_no_tls_client.hpp>

#include "libs/int128/int128.h"
typedef absl::uint128 uint128;

#include "libs/easyloggingpp/easylogging++.h"
#include "libs/json11/json11.hpp"
extern "C" 
{
	#include "libs/sr25519/src/sr25519.h"
}
using namespace json11;

using namespace std;

extern "C" {
    #include "utils/sr25519.h"
}

#include "polkacpp.h"

// Include structs
#include "structs/publickey.h"
#include "structs/hasher.h"
#include "structs/transfer.h"


#include "config.h"
#include "constants.h"
#include "errors.h"
#include "utils/converters.h"
#include "interfaces/imessageobserver.h"
#include "interfaces/iwsmessageobserver.h"
#include "interfaces/iwsclient.h"
#include "interfaces/ijsonrpc.h"
#include "interfaces/ilogger.h"

#include "utils/address.h"
#include "utils/storage.h"
#include "metadatafactory.h"
#include "wsclient.h"
#include "elog.h"
#include "application.h"

#include "jsonrpc.h"
// clang-format on
