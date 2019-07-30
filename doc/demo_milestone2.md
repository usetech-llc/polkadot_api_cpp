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
- All transaction types are supported, but for the demonstration sending DOT transaction is used in this E2E test
```
bin/submit_extrinsic <sender address> <recipient address> <amount in fDOTs> <sender private key (hex)>

for example:

bin/submit_extrinsic 5ECcjykmdAQK71qHBCkEWpWkoMJY6NXvpdKy8UeMx16q5gFr 5FpxCaAovn3t2sTsbBeT5pWTj2rg392E8QoduwAyENcPrKht 1000000000000000000 0xABCDEF123.....123
(private key was corrupted on purpose, both hex formats with or without leading 0x are supported)
```

Alternate RPC method with waiting - author_submitAndWatchExtrinsic
```
bin/submit_extrinsicNWatch <sender address> <recipient address> <amount in fDOTs> <sender private key (hex)>

for example:

bin/submit_extrinsicNWatch 5ECcjykmdAQK71qHBCkEWpWkoMJY6NXvpdKy8UeMx16q5gFr 5FpxCaAovn3t2sTsbBeT5pWTj2rg392E8QoduwAyENcPrKht 1000000000000000000 0xABCDEF123.....123
(private key was corrupted on purpose, both hex formats with or without leading 0x are supported)
```

author_pendingExtrinsics

The method may be examined with the following command with different parameters. Sometimes the default node (wss://alex.unfrastructure.io/public/ws) does not report any pending extrinsics. In order to get a non-empty result, we used alternate node: wss://poc3-rpc.polkadot.io/
```
bin/get_pending_extrinsics wss://alex.unfrastructure.io/public/ws
bin/get_pending_extrinsics wss://poc3-rpc.polkadot.io/
```

author_removeExtrinsic
So far this method is not yet supported by the node (response is "Method not found"), but when it becomes available, here is the way to test it. Now it can be tested too, and the output will inform if the method is supported or not. Parameters are the same as for transfer E2E test.
```
bin/remove_extrinsic <sender address> <recipient address> <amount in fDOTs> <sender private key (hex)>
```

### Support following WebSocket subscriptions

author_extrinsicUpdate

Method submit_extrinsicNWatch subscribes to extrinsicUpdate's.
