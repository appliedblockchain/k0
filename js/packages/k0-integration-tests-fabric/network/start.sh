#!/bin/bash

set -e

if [ "$CI" = "true" ]
then
  maybe_ci_dc_file="-f docker-compose-ci.yaml"
fi

echo Clearing everything...
sudo rm -rf crypto-config
docker rm $(docker ps -aq)

echo Creating crypto config...
docker run -v $PWD/crypto-config.yaml:/crypto-config.yaml:ro -v $PWD/crypto-config:/crypto-config hyperledger/fabric-tools:1.2.0 cryptogen generate --config=/crypto-config.yaml --output=/crypto-config
if [ "$CI" = "true" ]
then
  sudo chmod -R +r crypto-config
fi

echo Generating orderer genesis block...
docker run -it -v $PWD:/config hyperledger/fabric-tools:1.2.0 configtxgen -configPath /config -profile TheGenesis -channelID orderer-system-channel -outputBlock /config/artefacts/orderer_genesis.block

echo Starting network...
docker-compose -f docker-compose.yaml $maybe_ci_dc_file up -d

echo Waiting for CouchDBs...

bash -c 'while [[ "$(curl -s -o /dev/null -w ''%{http_code}'' localhost:11584)" != "200" ]]; do sleep 1; done'
bash -c 'while [[ "$(curl -s -o /dev/null -w ''%{http_code}'' localhost:12584)" != "200" ]]; do sleep 1; done'
bash -c 'while [[ "$(curl -s -o /dev/null -w ''%{http_code}'' localhost:13584)" != "200" ]]; do sleep 1; done'
bash -c 'while [[ "$(curl -s -o /dev/null -w ''%{http_code}'' localhost:14584)" != "200" ]]; do sleep 1; done'

echo Generating channel creation tx...

docker run -it -v $PWD:/config hyperledger/fabric-tools:1.2.0 configtxgen -configPath /config -profile TheChannel -channelID the-channel -outputCreateChannelTx ./config/artefacts/channel_creation.tx

echo Creating channel...

docker-compose run -w /artefacts alphatools peer channel create -o orderer.orderer.org:7050 -c the-channel -f channel_creation.tx --tls true --cafile /orderer/ca.crt

echo Joining channel...

for org in alpha beta gamma bank
do
  docker-compose run ${org}tools peer channel join -b /artefacts/the-channel.block
done

echo Generating anchor definition txs...

docker run -it -v $PWD:/config hyperledger/fabric-tools:1.2.0 configtxgen -configPath /config -profile TheChannel -channelID the-channel -outputAnchorPeersUpdate ./config/artefacts/alphaco_anchor_peers_definition.tx -asOrg AlphaCo
docker run -it -v $PWD:/config hyperledger/fabric-tools:1.2.0 configtxgen -configPath /config -profile TheChannel -channelID the-channel -outputAnchorPeersUpdate ./config/artefacts/betaco_anchor_peers_definition.tx -asOrg BetaCo
docker run -it -v $PWD:/config hyperledger/fabric-tools:1.2.0 configtxgen -configPath /config -profile TheChannel -channelID the-channel -outputAnchorPeersUpdate ./config/artefacts/gammaco_anchor_peers_definition.tx -asOrg GammaCo
docker run -it -v $PWD:/config hyperledger/fabric-tools:1.2.0 configtxgen -configPath /config -profile TheChannel -channelID the-channel -outputAnchorPeersUpdate ./config/artefacts/bankco_anchor_peers_definition.tx -asOrg BankCo

echo Sending anchor definition txs...

for org in alpha beta gamma bank; do docker-compose run ${org}tools peer channel update -o orderer.orderer.org:7050 -c the-channel -f /artefacts/${org}co_anchor_peers_definition.tx --tls true --cafile /orderer/ca.crt; done

echo Network started.
