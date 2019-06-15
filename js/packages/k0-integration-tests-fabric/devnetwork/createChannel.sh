#!/bin/sh

cd alphaadmin && peer channel create -o localhost:7050 -c the-channel -f ../artefacts/channel_creation.tx  --outputBlock ../artefacts/the-channel.block && \
cd ..
