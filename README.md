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

## Running the demo

### Preparation

before running the zk test:
```
mkdir /tmp/k0keys
```

In this section, you can either change the $ZKTRADING_PATH environment variable to your local zktrading project folder, or you can add this to your .bash_profile

```
export $ZKTRADING_PATH="YOUR OWN ABSOLUTE PATH TO THE ZKTRADING PROJECT FOLDER" # with no / at the end
```

Then, close and reopen your terminal and check that the variable is set with:

```
echo $ZKTRADING_PATH # should output your path
```

Generate proving keys and verification keys for the commitment, transfer, addition and withdrawal circuits:

```
mkdir /tmp/k0keys
for circuit in commitment transfer addition withdrawal;
 do $ZKTRADING_PATH/cpp/build/src/setup $circuit 7 /tmp/k0keys/${circuit}_pk /tmp/k0keys/${circuit}_vk && $ZKTRADING_PATH/cpp/build/src/convert_vk /tmp/k0keys/${circuit}_vk /tmp/k0keys/${circuit}_vk_alt; done
```

Running the demo(note: you will need 7 terminals, install iterm2 on mac for ease of use):

Run the proving servers(one terminal each):
```
 $ZKTRADING_PATH/cpp/build/src/server 7 /tmp/k0keys/commitment_pk /tmp/k0keys/commitment_vk /tmp/k0keys/addition_pk /tmp/k0keys/addition_vk /tmp/k0keys/transfer_pk /tmp/k0keys/transfer_vk /tmp/k0keys/withdrawal_pk /tmp/k0keys/withdrawal_vk 4000

$ZKTRADING_PATH/cpp/build/src/server 7 /tmp/k0keys/commitment_pk /tmp/k0keys/commitment_vk /tmp/k0keys/addition_pk /tmp/k0keys/addition_vk /tmp/k0keys/transfer_pk /tmp/k0keys/transfer_vk /tmp/k0keys/withdrawal_pk /tmp/k0keys/withdrawal_vk 5000

$ZKTRADING_PATH/cpp/build/src/server 7 /tmp/k0keys/commitment_pk /tmp/k0keys/commitment_vk /tmp/k0keys/addition_pk /tmp/k0keys/addition_vk /tmp/k0keys/transfer_pk /tmp/k0keys/transfer_vk /tmp/k0keys/withdrawal_pk /tmp/k0keys/withdrawal_vk 6000
```

Run the Merkle tree servers(1 terminal each):

```
$ZKTRADING_PATH/cpp/build/src/mtserver 7 4100
$ZKTRADING_PATH/cpp/build/src/mtserver 7 5100
$ZKTRADING_PATH/cpp/build/src/mtserver 7 6100
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


# Legacy README

## ZKP setup
```
rm -rf /tmp/k0keys && \
mkdir /tmp/k0keys && \
for circuit in commitment transfer addition withdrawal example; do \
cpp/build/src/setup $circuit 7 /tmp/k0keys/${circuit}_pk /tmp/k0keys/${circuit}_vk && \
~/go/src/github.com/appliedblockchain/zktrading/cpp/build/src/convert_vk /tmp/k0keys/${circuit}_vk /tmp/k0keys/${circuit}_vk_alt; \
done
```

## Run server
```
cd cpp/build
src/examples/merkle_tree/merkle_tree_server 2 /tmp/mt_addition_pk /tmp/mt_addition_vk \
                                              /tmp/mt_inclusion_pk /tmp/mt_inclusion_vk
```

## Run the tests

### CPP tests

```
cd cpp

# then

BASE_DIR=$(pwd) build/test/letest

# OR, to run only specific tests

BASE_DIR=$(pwd) build/test/letest --gtest_filter=EXPRESSION*
```

### JS tests
```
cd js
MOCHA_MERKLE_TREE_HEIGHT=2 node_modules/.bin/mocha test/mixer.js
```

## Fabric

The project needs to be checked out in the GOPATH (`$GOPATH/src/github.com/appliedblockchain/zktrading`)

ZKP setup as above

### Spin up a Fabric network
In `js/test/fabric/network`:
```
rm -rf artefacts/*
./start.sh
```

### Package chaincode
In `js/test/fabric/network`:
```
export CHAINCODE_VERSION=$(($CHAINCODE_VERSION+1)) && echo $CHAINCODE_VERSION

docker run \
-v $PWD/artefacts:/artefacts \
-v $GOPATH/src/github.com/hyperledger/fabric:/opt/gopath/src/github.com/hyperledger/fabric:ro \
-v $GOPATH/src/github.com/appliedblockchain/zktrading/fabric/chaincode/cash:/opt/gopath/src/github.com/appliedblockchain/fabric/chaincode/cash:ro \
hyperledger/fabric-tools:1.2.0 \
peer chaincode package \
-n k0chaincode -v $CHAINCODE_VERSION \
-p github.com/appliedblockchain/fabric/chaincode/cash \
/artefacts/k0chaincode.${CHAINCODE_VERSION}.out
```

### Install chaincode
In `js/test/fabric/network`:
```
for org in alpha beta gamma; \
do docker-compose run ${org}tools \
peer chaincode install /artefacts/k0chaincode.${CHAINCODE_VERSION}.out; done
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