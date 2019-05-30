package main

import (
	"fmt"
	"github.com/hyperledger/fabric/core/chaincode/shim"
	pb "github.com/hyperledger/fabric/protos/peer"
)

var logger = shim.NewLogger("k0Chaincode")

type K0Chaincode struct {
}

func (t *K0Chaincode) Init(stub shim.ChaincodeStubInterface) pb.Response {
	// TODO set initial Merkle tree root
	return shim.Success(nil)
}

func (t *K0Chaincode) Invoke(stub shim.ChaincodeStubInterface) pb.Response {
	args := stub.GetArgs()
	function := string(args[0])
	switch function {
	case "mint":
		return t.mint(stub, args[1:])
	case "transfer":
		return t.transfer(stub, args[1:])
	default:
		msg := "Function \"%s\" not (yet) implemented"
		return shim.Error(fmt.Sprintf(msg, function))
	}
}
