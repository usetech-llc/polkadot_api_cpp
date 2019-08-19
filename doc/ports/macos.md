# Building Polkadot C++ API on MacOS

## Prerequisites

```
git clone https://github.com/usetech-llc/polkadot_api_cpp.git ‑

brew install boost@1.60
brew install openssl
brew install cmake
```

Manually compile and install dependencies:
- for 64 bits platforms use flag -DCMAKE_OSX_ARCHITECTURES=x86_64
- for 32 bits platforms use flag  -DCMAKE_OSX_ARCHITECTURES=i386

```
git clone https://github.com/zaphoyd/websocketpp ‑

cmake -DBOOST_ROOT="/usr/local/opt/boost@1.60" -DCMAKE_OSX_ARCHITECTURES=x86_64 .
make install

git clone https://github.com/Warchant/sr25519-crust.git ‑

cmake -DBOOST_ROOT="/usr/local/opt/boost@1.60" -DCMAKE_OSX_ARCHITECTURES=x86_64 -DCMAKE_BUILD_TYPE=Release -DBUILD_SHARED_LIBS=TRUE .
make install
```

## Build project
```
cmake -DBOOST_ROOT="/usr/local/opt/boost@1.60" -DCMAKE_OSX_ARCHITECTURES=x86_64 .
make
```
