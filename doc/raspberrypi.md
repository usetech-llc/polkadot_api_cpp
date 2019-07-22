# How to Build on Raspberry Pi 3 (ARMv7)

The easiest way to build this API library for RaspberryPi is to cross-compile sr25519-crust on Linux and build the rest of API on Raspberry Pi.

1. Clone this fork of sr25519-crust: https://github.com/usetech-llc/sr25519-crust and follow build instructions in it. Copy built files to RPI

2. Install latest cmake on RPI:
```
git clone https://github.com/Kitware/cmake && cd cmake
./bootstrap && make -j4
sudo make install
cd
```

3. Install dependencies
```
sudo apt-get install libssl-dev libboost1.62-* libcurl4-openssl-dev
```

4. Clone and build
```
git clone https://github.com/usetech-llc/polkadot_api_cpp && cd polkadot_api_cpp
cmake .
make
make test
```
