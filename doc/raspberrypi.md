# How to Build on Raspberry Pi 3 (ARMv7)

1. Build sr25519-crust: https://github.com/usetech-llc/sr25519-crust

If you need help building it, please contact us at (am at usetech.com)

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
