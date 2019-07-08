# K0 Fabric integration tests

The project needs to be checked out in the GOPATH (`$GOPATH/src/github.com/appliedblockchain/k0`). You also need to have the Hyperledger Fabric source code (version 1.2.0) at `$GOPATH/src/github.com/hyperledger/fabric`.

Use node version 8, otherwise it will not work.

## ZKP setup
```
rm -rf /tmp/k0keys
mkdir /tmp/k0keys
for circuit in commitment transfer addition withdrawal example
do
  ../../../cpp/build/src/server $circuit 4 /tmp/k0keys/${circuit}\_pk /tmp/k0keys/${circuit}_vk
  ../../../cpp/build/src/convert_vk /tmp/k0keys/${circuit}\_vk /tmp/k0keys/${circuit}\_vk_alt
done
```

## Spin up a Fabric network

In subdirectory `network`:

```
rm -rf artefacts/*
./start.sh
```

## Remove previously created chaincode images

```
docker rmi $(docker images --filter=reference="*k0chaincode*" -q) || true
```

## Package chaincode

In `network`:

```
export CHAINCODE_VERSION=$(($CHAINCODE_VERSION+1)) && echo $CHAINCODE_VERSION

docker run -v $PWD/artefacts:/artefacts -v $GOPATH/src/github.com/hyperledger/fabric:/opt/gopath/src/github.com/hyperledger/fabric:ro -v $GOPATH/src/github.com/appliedblockchain/k0/go/:/opt/gopath/src/github.com/appliedblockchain/k0/go/:ro hyperledger/fabric-tools:1.2.0 peer chaincode package -n k0chaincode -v $CHAINCODE_VERSION -p github.com/appliedblockchain/k0/go/chaincode/cash /artefacts/k0chaincode.${CHAINCODE_VERSION}.out
```

## Install chaincode

In `network`:

```
for org in alpha beta gamma bank; do docker-compose run ${org}tools peer chaincode install /artefacts/k0chaincode.${CHAINCODE_VERSION}.out; done
```

## Instantiate chaincode

In this directory:

```
node instantiate
```

## Start proving and Merkle tree servers

In separate terminal windows, start the following servers:

Proving server:

```
../../../cpp/build/src/server 4 /tmp/k0keys/commitment_pk /tmp/k0keys/commitment_vk /tmp/k0keys/addition_pk /tmp/k0keys/addition_vk /tmp/k0keys/transfer_pk /tmp/k0keys/transfer_vk /tmp/k0keys/withdrawal_pk /tmp/k0keys/withdrawal_vk /tmp/k0keys/example_pk /tmp/k0keys/example_vk 11400
```

Merkle tree server for AlphaCo:

```
../../../cpp/build/src/mtserver 4 11410
```

Merkle tree server for BetaCo:

```
../../../cpp/build/src/mtserver 4 12410
```

../../../Merkle tree server for GammaCo:

```
cpp/build/src/mtserver 4 13410
```

Merkle tree server for BankCo:

```
../../../cpp/build/src/mtserver 4 14410
```

## Run tests

```
node_modules/.bin/mocha test
```

## Fabric Dev mode

For a faster way to itterate on chaincode development, use this setup, that will allow you to build and restart the chaincode without having to redeploy

### Pre Setup

First, start a ZKP setup as described [above](#zkp-setup)

Then, add the following line to your `/etc/hosts` file:

```
127.0.0.1        orderer.orderer.org
```
follow [those instruction](https://hyperledger-fabric.readthedocs.io/en/release-1.2/install.html) to install the fabric binaries

For our version, on mac, downloading the binary would look like this:
```
curl -sSL http://bit.ly/2ysbOFE | bash -s 1.2.0 1.2.0 1.2.0
```

Then, add the `fabric-samples/bin` directory to you `$PATH`

### Starting the network

__In this section, the command needs to be run from the directory `devnetwork`.__  

Use this command to clean previous artefacts, if any:  

```
docker-compose down && rm -rf crypto-config/* artefacts/* *peer/data && docker rm $(docker ps -qa)
```

Generate the crypto-config:
```
cryptogen generate --config=crypto-config.yaml
```

Generate the genesis block for fabric:
```
configtxgen -profile TheGenesis -channelID orderer-system-channel -outputBlock artefacts/orderer_genesis.block
```

Generate the channel config:

```
configtxgen -profile TheChannel -channelID the-channel -outputCreateChannelTx artefacts/channel_creation.tx
```

Start the orderer:
```
docker-compose up
```

Then open 12 terminals (iTerm2 or similar recommended), respectively in:
* `js/test/fabric/devnetwork/alphapeer`
* `js/test/fabric/devnetwork/betapeer`
* `js/test/fabric/devnetwork/gammaapeer`
* `js/test/fabric/devnetwork/bankpeer`
* `js/test/fabric/devnetwork/alphaadmin`
* `js/test/fabric/devnetwork/betaadmin`
* `js/test/fabric/devnetwork/gammaadmin`
* `js/test/fabric/devnetwork/bankadmin`
* __3__ in `go/chaincode/cash`


in alphaadmin, run:
```
peer channel create -o localhost:7050 -c the-channel -f ../artefacts/channel_creation.tx  --outputBlock ../artefacts/the-channel.block
```

in alphapeer, betapeer, gammapeer and bankpeer, run:
```
peer node start --peer-chaincodedev=true
```

in alphaadmin, betaadmin, gammaadmin and bankadmin, run:
```
peer channel join -b ../artefacts/the-channel.block
```

### Compiling the chaincode

in `go/chaincode/cash`

 `go build` then,

Start the chaincodes program by running each of those command in a separate terminal:

```
VERIFIER_ENDPOINT=http://localhost:11400/ CORE_CHAINCODE_LOGLEVEL=debug CORE_PEER_ADDRESS=localhost:11752 CORE_CHAINCODE_ID_NAME=k0chaincode:1 ./cash
VERIFIER_ENDPOINT=http://localhost:12400/ CORE_CHAINCODE_LOGLEVEL=debug CORE_PEER_ADDRESS=localhost:12752 CORE_CHAINCODE_ID_NAME=k0chaincode:1 ./cash
VERIFIER_ENDPOINT=http://localhost:13400/ CORE_CHAINCODE_LOGLEVEL=debug CORE_PEER_ADDRESS=localhost:13752 CORE_CHAINCODE_ID_NAME=k0chaincode:1 ./cash
VERIFIER_ENDPOINT=http://localhost:14400/ CORE_CHAINCODE_LOGLEVEL=debug CORE_PEER_ADDRESS=localhost:14752 CORE_CHAINCODE_ID_NAME=k0chaincode:1 ./cash
```

Finally, in alphaadmin, betaadmin, gammaadmin and bankadmin, run:

```
CORE_CHAINCODE_MODE=net peer chaincode install -p github.com/appliedblockchain/k0/go/chaincode/cash -n k0chaincode -v 1
```

### Instantiating and running the chaincode

```
DEV_MODE=true node instantiate
```

```
DEV_MODE=true mocha test.js
```