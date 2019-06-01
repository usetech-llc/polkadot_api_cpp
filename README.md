# Polkadot C++ API

## Install Prerequisites


### Build tools
```
sudo apt-get -y install build-essential cmake
```

### curl headers
```
sudo apt-get install libcurl4-openssl-dev
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
```

### Run connect basic example

Connect example will establish WebSocket connection to the polkadot node and read and output basic node information.
```
bin/polkaclient connect
```

### Read Address Balance Example

Balance example will establish WebSocket connection and read and output address balance in DOTs.
```
bin/polkaclient balance <your polkadot address>
```


## Running Unit Tests

```
make test
```

## How to Use

### Files to include

### Library Initialization

### Eastablishing and maintaining connection

### Reading data from polkadot node

### Sending extrincics
