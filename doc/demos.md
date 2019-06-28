# Manual Demos

## Deliverable 1

### The project can be built with provided instructions on bare installation of Ubuntu Linux 16.04.

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

### API can be subscribed and unsibscribed
```
bin/wssubscribeApp
```
