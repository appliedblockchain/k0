# K0 C++ components

## Set up the project dependencies

Linux:

```
sudo apt-get install -y \
build-essential cmake git libgmp3-dev libprocps-dev python-markdown \
libboost-all-dev libssl-dev libjsonrpccpp-dev libjsonrpccpp-tools

sudo ln -s /usr/include/jsoncpp/json/ /usr/include/json

git submodule update --init --recursive
```

Mac:

```
brew install https://raw.githubusercontent.com/ethereum/homebrew-ethereum/e05aa38c0f26874ec36409a83d1dbf83424552a4/solidity.rb # install solc 0.5.3
brew install libjson-rpc-cpp
brew install jsoncpp
brew install openssl
brew install sodium
```

### Install Google Test

- download https://github.com/google/googletest/releases/tag/release-1.8.1 to a directory
- unpack it with `mkdir gtest && tar -xzf release-1.8.1.tar.gz -C gtest/`
- build and install it:

```
cd gtest
mkdir build
cd build
cmake ..
make
make install
```

## Build

```
mkdir build
cd build
```

Then, on Linux:

```
cmake ..
```

Or, on Mac OS X (YMMV):

```
CPPFLAGS="-I/usr/local/opt/openssl/include -I/usr/local/include/sodium" \
LDFLAGS="-L/usr/local/opt/openssl/lib -L/usr/local/lib" \
PKG_CONFIG_PATH=/usr/local/opt/openssl/lib/pkgconfig \
cmake -DWITH_PROCPS=OFF -DWITH_SUPERCOP=OFF -DMULTICORE=OFF ..
```

Then:

```
make
```

(If you have multiple cores, try e.g. `make -j 4` for 4 cores).

## C++ tests

```
BASE_DIR=\$(pwd) build/test/letest

# OR, to run only specific tests

BASE_DIR=\$(pwd) build/test/letest --gtest_filter=EXPRESSION\*

```

