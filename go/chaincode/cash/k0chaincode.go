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
	var err error
	_, args := stub.GetFunctionAndParameters()
	if len(args) != 1 {
		return shim.Error("Error in Init: Incorrect number of " +
			"arguments. Expecting 1, got: " + string(len(args)))
	}
	logger.Infof("Setting value to: %s", args[0])
	err = stub.PutState("value", []byte(args[0]))
	if err != nil {
		return shim.Error(err.Error())
	}

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
