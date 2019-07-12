# Manual Demos

Milestone 1 deliverables are located in branch [release/milestone1](https://github.com/usetech-llc/polkadot_api_cpp/tree/release/milestone1)

## Deliverable 1

### The project can be built with provided instructions on bare installation of Ubuntu Linux 16.04.

First, please install prerequisites as described in [main README.md](https://github.com/usetech-llc/polkadot_api_cpp/blob/release/milestone1/README.md) file

```
cmake .
make
```

### Connection:
- Connection to a public Substrate node (at URL wss://poc3-rpc.polkadot.io:443/) can be established through WebSocket with API connect command.
- Connection to a public node can be closed with API disconnect command.

```
bin/connect
```

#### Node’s TLS certificate is verified during connection process

This unit test hides file 'ca-chain.cert.pem' that contains trusted root CA certificate making it impossible to verify node's TLS certificate:
```
bin/connect_failed_tls
```

### Basic data can be read from the node, deserialized to appropriate C++ struct and returned from API, which includes following Test-RPC commands:

#### chain_getBlockHash
```
bin/connectApp
```

#### chain_getRuntimeVersion (and alias state_getRuntimeVersion)
```
bin/connectApp
```

#### state_getMetadata
```
bin/get_metadata
```

#### system_properties, system_chain, system_name, system_version
```
bin/connectApp
```

### Command line tool is provided to execute all milestone deliverables

Currently unit tests, see above

### Building instructions, initialization, and library usage documented

- Building: See [root level README.md](https://github.com/usetech-llc/polkadot_api_cpp/blob/master/README.md)
- Lib usage: See doc/html folder


## Deliverable 2 - WebSocket Subscriptions

### API can be subscribed and unsibscribed. Current block subscription.
```
bin/wssubscribeApp
```

### Balance subscription
```
bin/wssubscribeBalance
```

Expect output such as:
```
...
2019-07-12 15:09:27,114 INFO [default] Subscribed with subscription ID: 2760311

  Balance: 201020000000000

2019-07-12 15:09:27,506 INFO [default] runWsMessages Thread exited
success
```

### Current era and session subscription
```
bin/wssubscribeAppEraNSession
```

## Deliverable 3 - Signing and Sending Transactions

- One transaction type is supported - sending DOTs to another address
- Transaction is serialized and prepared (formatted) appropriately for signing
- Transaction can be signed with provided private key
- Cryptogram can be sent to the substrate node to be processed and included in the blockchain
- Command line tool is provided to execute all milestone deliverables
```
bin/transfer <sender address> <recipient address> <amount in fDOTs> <sender private key (hex)>

for example:

bin/transfer 5ECcjykmdAQK71qHBCkEWpWkoMJY6NXvpdKy8UeMx16q5gFr 5FpxCaAovn3t2sTsbBeT5pWTj2rg392E8QoduwAyENcPrKht 1000000000000000000 0xABCDEF123.....123
(private key was corrupted on purpose, both hex formats with or without leading 0x are supported)
```

Expect output such as:
```
2019-07-12 15:04:24,865 INFO [default] Message received: {"jsonrpc":"2.0","method":"author_extrinsicUpdate","params":{"result":{"finalized":"0x37361e7f88a9105b103b32458e2748ec4758ec8dca733da61c1403d9bda70d42"},"subscription":2758756}}


   ---=== Transaction was mined! ===---


2019-07-12 15:04:25,174 INFO [default] runWsMessages Thread exited
success
```
