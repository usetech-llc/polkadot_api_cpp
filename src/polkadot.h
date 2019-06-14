#include <iostream>
#include <string>
#include <curl/curl.h>

// Websockets
#include <websocketpp/config/asio_client.hpp>
#include <websocketpp/client.hpp>





using namespace std;

#include "constants.h"
#include "wsclient.h"

#include "json11.hpp"
using namespace json11;

void PrintFromLib();
int Foo(int bar);
