# Manual Demos

Milestone 2 deliverables are located in branch [release/milestone2](https://github.com/usetech-llc/polkadot_api_cpp/tree/release/milestone2)

The project can be built with provided instructions on bare installation of Ubuntu Linux 16.04.

For convenience the Dockerfile is provided that installs all dependencies as needed, checks out the code for the API and builds it. In order to execute single API tests manually, please have docker CE installed, then clone this repository and run following commands (first one takes ~20 minutes to run for the first time):
```
$ docker build -t cppapi .
$ docker run -it --rm cppapi /bin/bash
# cd polkadot_api_cpp
```

Now you are connected to a running docker container with API built, tested, and ready. You can execute following commands to examine deliverables.

## Deliverable 4 - Expanded Support of Chain State

### Support following RPC methods

chain_getHeader, chain_getBlock
```
bin/get_block_ok
```

chain_getFinalizedHead
```
bin/get_blockfinalhead_ok
```

state_call
The method is available in API (just in case), but without test. Per Gavin on July 23 2019:
"it's probably fine to omit it for now, yeah.
if anyone wanted to make a parachain collator in c++, then it could be useful since you'd need it to query consensus stuff
but it's not especially useful for general applications"

state_getKeys
```
bin/get_keys
```

state_getStorage
```
bin/get_storage
```

state_getStorageHash
```
bin/get_storage_hash
```

state_getStorageSize
```
bin/get_storage_size
```

state_getChildKeys
state_getChildStorage
state_getChildStorageHash
state_getChildStorageSize

Currently these methods are not available. The node returns error: {"code":-32601,"message":"Method not found"}. We implemented them with hex-string child storage key and storage key parameters and they will return string response as is from the RPC call, but there are no concrete examples in unit tests since none of them would work anyway.
```
bin/get_child_storage
```

state_queryStorage

This E2E test queries timestamp from last 10 blocks. All timestamps must be different and there should be at least 10 of them.
```
bin/query_storage
```

system_health
```
bin/get_systemhealth_ok
```

system_peers
```
bin/get_systempeers_ok
```

system_networkState
```
bin/get_system_networkstate_ok
```

### Support following WebSocket subscriptions:

chain_newHead
```
bin/wssubscribeApp
```

chain_finalizedHead, state_runtimeVersion, state_storage
```
bin/wssubscribe_all
```

## Deliverable 4 - Expanded Support of Transactions

### Support following RPC methods

author_submitExtrinsic
author_pendingExtrinsics
author_removeExtrinsic
```
TBD
```

### Support following WebSocket subscriptions

author_extrinsicUpdate
```
TBD
```
