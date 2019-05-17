# ZKTRADING

## Set up the project dependencies
```
sudo apt-get install -y \
build-essential cmake git libgmp3-dev libprocps-dev python-markdown \
libboost-all-dev libssl-dev libjsonrpccpp-dev libjsonrpccpp-tools
sudo ln -s /usr/include/jsoncpp/json/ /usr/include/json
git submodule update --init --recursive
cd js
npm i
cd ..
```

or, on mac:


```
brew install https://raw.githubusercontent.com/ethereum/homebrew-ethereum/e05aa38c0f26874ec36409a83d1dbf83424552a4/solidity.rb # install solc 0.5.3
brew install libjson-rpc-cpp
brew install jsoncpp
brew install openssl

# probably some other to figure out, can all be installed by brew

```

### install google test:

- download https://github.com/google/googletest/releases/tag/release-1.8.1 to a directory
- unpack it with `mkdir gtest && tar -xzf release-1.8.1.tar.gz -C gtest/ `
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
cd cpp
mkdir build
cd build
```
Then, on Linux:
```
cmake ..
```
Or, on Mac OS X:
```
CPPFLAGS="-I/usr/local/opt/openssl/include -I/usr/local/include/sodium"  \
LDFLAGS="-L/usr/local/opt/openssl/lib -L/usr/local/lib" \
PKG_CONFIG_PATH=/usr/local/opt/openssl/lib/pkgconfig \
cmake -DWITH_PROCPS=OFF -DWITH_SUPERCOP=OFF -DMULTICORE=OFF ..
```
Then:
```
make
cd ../..
```


In certain cases, on mac, pkgconfig can't find open ssl, this can be fixed by running:

```
cd /usr/local/include
ln -s ../opt/openssl/include/openssl .
```

and then running the cmake and make commands again.

## Running the demo

### Preparation

before running the zk test:

```
cd "Your ZK dir"
mkdir /tmp/k0keys
```

Generate proving keys and verification keys for the commitment, transfer, addition and withdrawal circuits:

```
mkdir /tmp/k0keys
for circuit in commitment transfer addition withdrawal;
  do cpp/build/src/setup $circuit 7 /tmp/k0keys/${circuit}_pk /tmp/k0keys/${circuit}_vk && cpp/build/src/convert_vk /tmp/k0keys/${circuit}_vk /tmp/k0keys/${circuit}_vk_alt;
done
```

Running the demo(note: you will need 7 terminals, install iterm2 on mac for ease of use):

Run the proving servers(one terminal each):
```
cpp/build/src/server 7 /tmp/k0keys/commitment_pk /tmp/k0keys/commitment_vk /tmp/k0keys/addition_pk /tmp/k0keys/addition_vk /tmp/k0keys/transfer_pk /tmp/k0keys/transfer_vk /tmp/k0keys/withdrawal_pk /tmp/k0keys/withdrawal_vk 4000

cpp/build/src/server 7 /tmp/k0keys/commitment_pk /tmp/k0keys/commitment_vk /tmp/k0keys/addition_pk /tmp/k0keys/addition_vk /tmp/k0keys/transfer_pk /tmp/k0keys/transfer_vk /tmp/k0keys/withdrawal_pk /tmp/k0keys/withdrawal_vk 5000

cpp/build/src/server 7 /tmp/k0keys/commitment_pk /tmp/k0keys/commitment_vk /tmp/k0keys/addition_pk /tmp/k0keys/addition_vk /tmp/k0keys/transfer_pk /tmp/k0keys/transfer_vk /tmp/k0keys/withdrawal_pk /tmp/k0keys/withdrawal_vk 6000
```

Run the Merkle tree servers(1 terminal each):

```
cpp/build/src/mtserver 7 4100
cpp/build/src/mtserver 7 5100
cpp/build/src/mtserver 7 6100
```

Running Parity(in another terminal):

```
docker run -p 8545:8545 -p 8546:8546 appliedblockchain/parity-solo-instant
```

Init the js folder

```
cd $ZKTRADING_PATH/js
npm install
node init # create keys and addresses
```

init the state
```
cd demo
node deposit alice
node deposit bob

```

then, in separate terminals:

```
node wallet alice
node wallet bob
node wallet carol
```

(Video example of how to use the demo)[https://www.youtube.com/watch?v=h2KyMOdnbtI].


for circuit in commitment transfer addition withdrawal;
  do cpp/build/src/setup $circuit 5 /tmp/k0keys/${circuit}_pk /tmp/k0keys/${circuit}_vk && $ZKTRADING_PATH/cpp/build/src/convert_vk /tmp/k0keys/${circuit}_vk /tmp/k0keys/${circuit}_vk_alt;
done
