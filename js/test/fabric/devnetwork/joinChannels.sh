#!/bin/sh

set -e

cd alphaadmin && peer channel join -b ../artefacts/the-channel.block && \
cd ../betaadmin && peer channel join -b ../artefacts/the-channel.block && \
cd ../gammaadmin && peer channel join -b ../artefacts/the-channel.block && \
cd ../bankadmin && peer channel join -b ../artefacts/the-channel.block && \
cd ..
