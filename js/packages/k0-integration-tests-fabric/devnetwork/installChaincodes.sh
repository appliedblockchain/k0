#!/bin/sh

cd alphaadmin && CORE_CHAINCODE_MODE=net peer chaincode install -p github.com/appliedblockchain/zktrading/go/chaincode/cash -n k0chaincode -v 1 && \
cd ../betaadmin && CORE_CHAINCODE_MODE=net peer chaincode install -p github.com/appliedblockchain/zktrading/go/chaincode/cash -n k0chaincode -v 1 && \
cd ../gammaadmin && CORE_CHAINCODE_MODE=net peer chaincode install -p github.com/appliedblockchain/zktrading/go/chaincode/cash -n k0chaincode -v 1 && \
cd ../bankadmin && CORE_CHAINCODE_MODE=net peer chaincode install -p github.com/appliedblockchain/zktrading/go/chaincode/cash -n k0chaincode -v 1 && \
cd ..
