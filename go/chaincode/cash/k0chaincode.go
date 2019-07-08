package main

import (
	"fmt"

	"github.com/appliedblockchain/k0/go/util"
	"github.com/hyperledger/fabric/core/chaincode/lib/cid"
	"github.com/hyperledger/fabric/core/chaincode/shim"
	pb "github.com/hyperledger/fabric/protos/peer"
)

var logger = shim.NewLogger("k0Chaincode")

type K0Chaincode struct {
}

func (t *K0Chaincode) Init(stub shim.ChaincodeStubInterface) pb.Response {
	fncName, args := stub.GetFunctionAndParameters()

	if fncName != "init" {
		str := "Expected function name \"init\", got \"%s\""
		return shim.Error(fmt.Sprintf(str, fncName))
	}

	if len(args) != 1 {
		str := "Incorrect number of arguments. Expecting 1, got: %d"
		return shim.Error(fmt.Sprintf(str, len(args)))
	}

	// Setting initial Root
	initialRoot, err := util.VariableToFixed32([]byte(args[0]))

	err = stub.PutState("root", initialRoot[:])
	if err != nil {
		return shim.Error(err.Error())
	}

	numLeaves, err := util.UintToBytes8(0)
	if err != nil {
		return shim.Error(err.Error())
	}

	err = stub.PutState("numLeaves", numLeaves[:])
	if err != nil {
		return shim.Error(err.Error())
	}

	mspid, err := cid.GetMSPID(stub)
	if err != nil {
		return shim.Error(err.Error())
	}
	err = stub.PutState("minterID", []byte(mspid))
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
	case "getState":
		return t.getState(stub, args[1:])
	default:
		msg := "Function \"%s\" not (yet) implemented"
		return shim.Error(fmt.Sprintf(msg, function))
	}
}
