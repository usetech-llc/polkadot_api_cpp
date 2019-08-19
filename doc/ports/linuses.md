# Building Polkadot C++ API on Different distros of Linux

## Manjaro and Arch Linux

### Versions:

- Manjaro Linux (32-Bit)  
- Manjaro Linux
- archlinux-2019.08.01-x86_64.iso
- archlinux-2019.08.02-dual.iso

### Prerequisites (both 32-Bit and x64)
```
pacman -Ss boost
pacman -Ss curl
sudo pacman -S openssl
sudo pacman -S cmake
sudo pacman -S gcc
sudo pacman -S extra/boost
sudo pacman -S extra/boost-libs
```

Also, you may need to download boost c++ for linux from https://www.boost.org, then copy /boost folder into /usr/lib.

Install Rustup:
```
curl --proto '=https' --tlsv1.2 -sSf https://sh.rustup.rs | sh
cmake -DBOOST_ROOT=path .
sudo make deps
```

#### x64

Manually copy header file (sr25519.h) into /usr/include and lib files (build/release/libsr25519crust.*) into /usr/lib64 or /usr/lib32 (depending on yours OS achirecture).

## OpenSuse

### Versions

- x86_64
- i586

### Prerequisites (both 32-Bit and x64)

```
sudo bash
yast -> install gcc with GUI
# or
sudo apt-get install gcc

sudo apt-get install boost-devel
sudo apt-get install curl-devel
sudo apt-get install libopenssl-devel
sudo apt-get install cargo
curl --proto '=https' --tlsv1.2 -sSf https://sh.rustup.rs | sh
```

## Debian

### Versions
- Debian 10 i386
- debian-10.0.0-amd64-xfce-CD-1.iso

#### (x64)
```
sudo apt-get install libssl-dev
sudo apt-get install curl
```

Edit /etc/sudoers configuration file (otherwise "make deps" will not work):
```
Defaults env_keep += HOME
```

#### (i386)

Manually install cargo and rust:
```
sudo apt-get install cargo
curl --proto '=https' --tlsv1.2 -sSf https://sh.rustup.rs | sh
```
