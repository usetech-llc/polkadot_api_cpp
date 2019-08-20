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
Note that we are using c8df32 commit of sr25519 library, which is the last "verified" at the moment. The most recent commit was breaking signature test. 
```
make deps
```

## Installation

### Installation from deb package

Download PolkadotCppAPI-0.1.1-Linux.deb from release files attached to this release and run
```
sudo dpkg -i PolkadotCppAPI-0.1.1-Linux.deb
```

### Manual installation (linux)

Download polkacpp.tar.gz from release files attached to this release and copy all files to the system root preserving directory structure from the archive.

### Building
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

### Transfer DOTs Example

```
trasnfer <from address> <to address> <amount> <private key file>
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
auto api = polkadot::api::getInstance()->app();
```

### Establishing and maintaining connection
This call will establish connection and start message thread. It will also ping the node several times per minute with "health" request to keep the connection alive.
```
api->connect("wss://poc3-rpc.polkadot.io:443/");
```

### Reading data from polkadot node
The data is returned as C structures. This example shows how to read (and output) system info:
```
auto systemInfo = api->getSystemInfo();

cout << "  Chain ID       : " << systemInfo->chainId << endl
     << "  Chain Name     : " << systemInfo->chainName << endl
     << "  Token Decimals : " << systemInfo->tokenDecimals << endl
     << "  Token Symbol   : " << systemInfo->tokenSymbol << endl;
```

### Subscribing for updates

Subscribing is done by calling a matching subscribe method with parameters (if needed) and a callback (or a lambda function). This example subscribes for balance updates and prints new balance when it arrives:
```
api->subscribeBalance(address, [&](uint128 balance) {
    cout << endl << "  Balance: " << (uint64_t)balance << endl << endl;
});
```

In order to stop subscription, call matching unsubsribe method:
```
api->unsubscribeBalance();
```

### Sending transactions (signed Extrinsics)

This example sends some DOTs between addresses and waits for execution. Transaction is signed with private key.
```
api->signAndSendTransfer(senderAddr, senderPrivateKeyStr, recipientAddr, amount, [&](string result) {
    if (result == "ready")
        cout << endl << endl << "   ---=== Transaction was registered in network ===--- " << endl << endl << endl;
    if (result == "finalized") {
        cout << endl << endl << "   ---=== Transaction was mined! ===--- " << endl << endl << endl;
        done = true;
    }
});
```

### Finishing work

When connection is not needed anymore, call
```
api->disconnect();
```

### More Information

The complete documentation of API interface can be found at this starting point:
https://htmlpreview.github.io/?https://github.com/usetech-llc/polkadot_api_cpp/blob/master/doc/html/classIApplication.html

For more examples see:
- [examples](https://github.com/usetech-llc/polkadot_api_cpp/tree/master/examples) folder,
- [test](https://github.com/usetech-llc/polkadot_api_cpp/tree/master/test) folder with unit and E2E tests,
- [polkadot_ui](https://github.com/usetech-llc/polkadot_ui) repository, which implements a GTK 3.0 UI and uses this API.
