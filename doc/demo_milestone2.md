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
```
TBD
```

state_getKeys
```
TBD
```

state_getStorage
```
TBD
```

state_getStorageHash
state_getStorageSize
state_getChildKeys
state_getChildStorage
state_getChildStorageHash
state_getChildStorageSize
state_queryStorage
```
TBD
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
