// clang-format off
#include <curl/curl.h>
#include <iostream>
#include <string>
#include <thread>
#include <unistd.h>

// Websockets
#include <websocketpp/client.hpp>
#include <websocketpp/config/asio_client.hpp>

using namespace std;

#include "constants.h"
#include "interfaces/iwsmessageobserver.h"
#include "interfaces/iwsclient.h"
#include "interfaces/ilogger.h"
#include "wsclient.h"
#include "loggerstub.h"

#include "libs/json11/json11.hpp"
using namespace json11;
#include "jsonrpc.h"
// clang-format on
