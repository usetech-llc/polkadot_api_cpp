#include <curl/curl.h>
#include <iostream>
#include <string>

// Websockets
#include <websocketpp/client.hpp>
#include <websocketpp/config/asio_client.hpp>

using namespace std;

#include "constants.h"
#include "wsclient.h"

#include "libs/json11/json11.hpp"
using namespace json11;
