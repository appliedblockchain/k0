# ZKTRADING

If you want to run the Fabric tests/demo, the project should be checked out in your GOPATH (eg. at `~/go/src/github.com/appliedblockchain/zktrading`).

[Set up the project dependencies](#set-up-the-project-dependencies)  
[Build](#build)  
[C++ tests](#c-tests)  
[ZKP setup](#zkp-setup-needed-for-all-tests-and-demos)  
[Ethereum demo](#ethereum-demo)  
[Fabric integration tests](#fabric-integration-tests)  

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

# probably some other to figure out, can all be installed by brew
```

### Install Google Test

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

### Install JS dependencies
```
cd js
npm i
cd ..
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
CPPFLAGS="-I/usr/local/opt/openssl/include -I/usr/local/include/sodium" \
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


## C++ tests

```
cd cpp

# then

BASE_DIR=$(pwd) build/test/letest

# OR, to run only specific tests

BASE_DIR=$(pwd) build/test/letest --gtest_filter=EXPRESSION*
```


## ZKP setup (needed for all tests and demos)
```
rm -rf /tmp/k0keys && \
mkdir /tmp/k0keys && \
for circuit in commitment transfer addition withdrawal example; do \
cpp/build/src/setup $circuit 7 /tmp/k0keys/${circuit}_pk /tmp/k0keys/${circuit}_vk && \
cpp/build/src/convert_vk /tmp/k0keys/${circuit}_vk /tmp/k0keys/${circuit}_vk_alt; \
done
```


## Ethereum demo

ZKP setup as described [above](#zkp-setup-needed-for-all-tests-and-demos)

Note: you will need 7 terminals (consider using iTerm2 on Mac for ease of use).

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

Running Parity (in another terminal):

```
docker run -p 8545:8545 -p 8546:8546 appliedblockchain/parity-solo-instant
```

Init the js folder

```
cd js
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

[Video example of how to use the demo](https://www.youtube.com/watch?v=h2KyMOdnbtI)


## Fabric integration tests

The project needs to be checked out in the GOPATH (`$GOPATH/src/github.com/appliedblockchain/zktrading`)

ZKP setup as described [above](#zkp-setup-needed-for-all-tests-and-demos)

### Spin up a Fabric network
In `js/test/fabric/network`:
```
rm -rf artefacts/*
./start.sh
```

### Remove previously created chaincode images
```
docker rmi $(docker images --filter=reference="*k0chaincode*" -q) || true
```

### Package chaincode
In `js/test/fabric/network`:
```
export CHAINCODE_VERSION=$(($CHAINCODE_VERSION+1)) && echo $CHAINCODE_VERSION

docker run -v $PWD/artefacts:/artefacts -v $GOPATH/src/github.com/hyperledger/fabric:/opt/gopath/src/github.com/hyperledger/fabric:ro -v $GOPATH/src/github.com/appliedblockchain/zktrading/fabric/chaincode/cash:/opt/gopath/src/github.com/appliedblockchain/fabric/chaincode/cash:ro hyperledger/fabric-tools:1.2.0 peer chaincode package -n k0chaincode -v $CHAINCODE_VERSION -p github.com/appliedblockchain/fabric/chaincode/cash /artefacts/k0chaincode.${CHAINCODE_VERSION}.out
```

### Install chaincode
In `js/test/fabric/network`:
```
for org in alpha beta gamma; do docker-compose run ${org}tools peer chaincode install /artefacts/k0chaincode.${CHAINCODE_VERSION}.out; done
```

### Instantiate chaincode
In `js/test/fabric`:
```
node instantiate
```

### Start proving and Merkle tree servers
In separate terminal windows, start the following servers:

Proving server for AlphaCo:
```
cpp/build/src/server 7 /tmp/k0keys/commitment_pk /tmp/k0keys/commitment_vk /tmp/k0keys/addition_pk /tmp/k0keys/addition_vk /tmp/k0keys/transfer_pk /tmp/k0keys/transfer_vk /tmp/k0keys/withdrawal_pk /tmp/k0keys/withdrawal_vk /tmp/k0keys/example_pk /tmp/k0keys/example_vk 11400
```

Proving server for BetaCo:
```
cpp/build/src/server 7 /tmp/k0keys/commitment_pk /tmp/k0keys/commitment_vk /tmp/k0keys/addition_pk /tmp/k0keys/addition_vk /tmp/k0keys/transfer_pk /tmp/k0keys/transfer_vk /tmp/k0keys/withdrawal_pk /tmp/k0keys/withdrawal_vk /tmp/k0keys/example_pk /tmp/k0keys/example_vk 12400
```

Proving server for GammaCo:
```
cpp/build/src/server 7 /tmp/k0keys/commitment_pk /tmp/k0keys/commitment_vk /tmp/k0keys/addition_pk /tmp/k0keys/addition_vk /tmp/k0keys/transfer_pk /tmp/k0keys/transfer_vk /tmp/k0keys/withdrawal_pk /tmp/k0keys/withdrawal_vk /tmp/k0keys/example_pk /tmp/k0keys/example_vk 13400
```

Merkle tree server for AlphaCo:
```
cpp/build/src/mtserver 7 11410
```

Merkle tree server for BetaCo:
```
cpp/build/src/mtserver 7 12410
```

Merkle tree server for GammaCo:
```
cpp/build/src/mtserver 7 13410
```

### Run tests
In `js`:
```
node_modules/.bin/mocha test/fabric/test.js
```