## Set up dependencies etc
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

## Run tests
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