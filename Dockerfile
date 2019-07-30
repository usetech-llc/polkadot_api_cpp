FROM ubuntu:16.04

MAINTAINER Greg Zaitsev <gz@usetech.com>

# Install build tools
RUN apt-get update && apt-get -y install libboost-all-dev
RUN apt-get -y install git build-essential
RUN apt-get -y install libcurl4-openssl-dev
RUN apt-get -y install libssl-dev
RUN apt-get -y install curl wget

# Install cmake 3.14 (required by sr25519-crust)
RUN wget https://github.com/Kitware/CMake/releases/download/v3.14.5/cmake-3.14.5.tar.gz && tar -xvf cmake-3.14.5.tar.gz
RUN cd cmake-3.14.5 && ./bootstrap && make -j4 && make install
RUN cmake --version

# Install websocketpp
RUN git clone https://github.com/zaphoyd/websocketpp wpp && mv wpp/websocketpp /usr/local/include/websocketpp

# Build and install sr25519-crust
RUN curl https://sh.rustup.rs -sSf | sh -s -- -y --default-toolchain nightly
RUN bash -c 'source $HOME/.cargo/env; rustup install nightly && rustup default nightly'
RUN bash -c 'source $HOME/.cargo/env; rustup --version'
RUN git clone https://github.com/Warchant/sr25519-crust.git sr25519
RUN bash -c 'source $HOME/.cargo/env; cd sr25519 && mkdir build && cd build && cmake .. -DCMAKE_BUILD_TYPE=Release && make install'

# Build Polkadot C++ API
RUN git clone https://github.com/usetech-llc/polkadot_api_cpp.git
RUN cd polkadot_api_cpp && git fetch --all && git checkout release/milestone2
RUN cd polkadot_api_cpp && cmake . && make && make test
