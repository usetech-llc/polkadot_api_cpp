// clang-format off
#include <curl/curl.h>
#include <functional>
#include <iostream>
#include <string>
#include <thread>
#include <unistd.h>

// Websockets
#include <websocketpp/client.hpp>
#include <websocketpp/config/asio_client.hpp>

#include "libs/easyloggingpp/easylogging++.h"
#include "libs/json11/json11.hpp"
#include "scale.h"
using namespace json11;

using namespace std;

// Include structs
#include "structs/responsebase.h"
#include "structs/blockhash.h"
#include "structs/runtimeversion.h"
#include "structs/metadataV0.h"
#include "structs/metadataV5.h"
#include "structs/metadata.h"
#include "structs/systeminfo.h"
#include "structs/publickey.h"
#include "structs/hasher.h"
#include "structs/common.h"
#include "structs/transfer.h"

#include "constants.h"
#include "errors.h"
#include "interfaces/imessageobserver.h"
#include "interfaces/iwsmessageobserver.h"
#include "interfaces/iwsclient.h"
#include "interfaces/ijsonrpc.h"
#include "interfaces/ilogger.h"
#include "interfaces/iapplication.h"
#include "utils/address.h"
#include "utils/storage.h"
extern "C" {
    #include "utils/sr25519.h"
}
#include "metadatafactory.h"
#include "wsclient.h"
#include "elog.h"
#include "application.h"

#include "jsonrpc.h"
// clang-format on
