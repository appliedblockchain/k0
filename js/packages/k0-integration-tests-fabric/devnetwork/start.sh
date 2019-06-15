#!/bin/bash

ORIGINAL_DIR=${pwd}
DIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd )"

cd $DIR
docker-compose down && rm -rf crypto-config/* artefacts/* *peer/data && docker rm $(docker ps -qa)
cryptogen generate --config=crypto-config.yaml
configtxgen -profile TheGenesis -channelID orderer-system-channel -outputBlock artefacts/orderer_genesis.block
configtxgen -profile TheChannel -channelID the-channel -outputCreateChannelTx artefacts/channel_creation.tx
docker-compose up -d

cd $ORIGINAL_DIR
