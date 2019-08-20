# Building Polkadot C++ API on MacOS

## Prerequisites

Install x-code command line tools, homebrew tool, and rustc first, then run in terminal:

```
git clone https://github.com/usetech-llc/polkadot_api_cpp.git ‑

brew install boost@1.60
brew install openssl
brew install cmake
export LDFLAGS="-L/usr/local/opt/openssl/lib -L/usr/local/lib"
export CPPFLAGS="-I/usr/local/opt/openssl/include"
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
export LIBRARY_PATH=/usr/local/lib/
```

## Build project
```
cmake -DBOOST_ROOT="/usr/local/opt/boost@1.60" -DOPENSSL_ROOT_DIR="/usr/local/opt/openssl" -DCMAKE_OSX_ARCHITECTURES=x86_64 .
make
```
