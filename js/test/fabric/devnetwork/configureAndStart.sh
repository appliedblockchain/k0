set -e
cryptogen generate --config=crypto-config.yaml
sleep 1
configtxgen -profile TheGenesis -channelID orderer-system-channel -outputBlock artefacts/orderer_genesis.block
sleep 1
configtxgen -profile TheChannel -channelID the-channel -outputCreateChannelTx artefacts/channel_creation.tx
sleep 1
docker-compose up
