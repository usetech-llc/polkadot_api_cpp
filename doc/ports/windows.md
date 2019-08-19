# Building Polkadot C++ API on Windows

## Prerequisites

- MSBuild tools (Visual Studio Installer; Visual Studio C++ and build tools) (tested with VS 2017)
- Boost library - https://www.boost.org (tested with version 1.65)
- zlib library - http://gnuwin32.sourceforge.net/packages/zlib.htm (tested with version 1.2.3)
- openssl sdk - https://slproweb.com/products/Win32OpenSSL.html (tested with Win64 OpenSSL v1.1.1c MSI, experimental)
- curl - http://www.confusedbycode.com/curl/
- cmake (tested with 3.15.0-rc1)
- Rust https://rustup.rs/

## Build

### In command line run to install nightly build of Rust:
```
rustup install nightly
```

### Clone and install library dependencies:

In c:/websocketpp run
```
git clone https://github.com/zaphoyd/websocketpp ‑
```

In project folder run
```
git clone https://github.com/Warchant/sr25519-crust.git ‑
```

Find available platforms and archirecture, then choose one and generate files. For example: -G"Visual Studio 15 2017 Win64"
```
cmake --help
```

Manually build sr25519 (in sr25519 folder):
```
cmake -G"Visual Studio 15 2017 Win64" -DCMAKE_BUILD_TYPE=Release .
cmake --build .
```

Copy lib files into polkadot project folder.

### Build C++ API

Run in project folder:
```
cmake -G"Visual Studio 15 2017 Win64" .
cmake --build .
```
