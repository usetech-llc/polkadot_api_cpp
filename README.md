# Polkadot Substrate C++ API

## Install Prerequisites

### Boost
```
sudo apt-get install libboost-all-dev
```

### Build tools
```
sudo apt-get -y install build-essential cmake
```

### curl headers
```
sudo apt-get install libcurl4-openssl-dev
```

### doxygen
```
sudo apt-get install doxygen python-pydot python-pydot-ng graphviz
```

### Other dependencies: websocketpp, sr25519-crust...
```
make deps
```

## Installation

This is work in progress, so installation is only possible from the source code.
Here are instructions how to compile the library and run examples:

### Checkout and build
```
git clone https://github.com/usetech-llc/polkadot_api_cpp
cd polkadot_api_cpp
cmake .
make
sudo make install
```

### Build "clip" example client
```
make clip
```

### Run connect basic example

Connect example will establish WebSocket connection to the polkadot node and read and output basic information.
```
bin/clip connect
```

### Read Address Balance Example

Balance example will establish WebSocket connection and read and output address balance in DOTs.
```
bin/clip balance <your polkadot address>
```


## Running Unit Tests

```
make test
```

## How to Use

### Files to include
```
#include <polkadotcpp/polkadot.h>
```

### Files to link
```
/usr/lib/polkadotcpp/libpolkacpp.a
```

### Library Initialization
Parity node URL is the only required parameter, though URL must include port. Example:
```
string parity_node_url("wss://poc3-rpc.polkadot.io:443/");
CWebSocketClient ws(parity_node_url);
```

### Eastablishing and maintaining connection
This call will establish connection and start message thread:
```
ws.connect();
```

The websocket subscriptions will provide data via the callback. In order to subscribe to an endpoint, call:
```
ws.subscribe(endpoint);
```

In order to stop subscription, call:
```
ws.unsubscribe(endpoint);
```

When connection is not needed anymore, call
```
ws.disconnect();
```

### Reading data from polkadot node
The data is returned as C structures. Documentation TBD.

### Sending extrincics
