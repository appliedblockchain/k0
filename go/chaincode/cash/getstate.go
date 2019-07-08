package main

import (
	"github.com/appliedblockchain/k0/go/util"
	"github.com/hyperledger/fabric/core/chaincode/shim"
	pb "github.com/hyperledger/fabric/protos/peer"
)

func (t *K0Chaincode) getState(
	stub shim.ChaincodeStubInterface,
	args [][]byte,
) pb.Response {
	numLeaves, _ := stub.GetState("numLeaves")
	root, _ := stub.GetState("root")

	state := util.ConcatByteSlices([][]byte{
		root,
		numLeaves,
	})

	return shim.Success(state)
}
