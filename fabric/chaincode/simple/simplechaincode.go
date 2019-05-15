package main

import (
	"fmt"
	"github.com/hyperledger/fabric/core/chaincode/shim"
	pb "github.com/hyperledger/fabric/protos/peer"
)

var logger = shim.NewLogger("k0Chaincode")

type SimpleChaincode struct {
}

func (t *SimpleChaincode) Init(stub shim.ChaincodeStubInterface) pb.Response {
	logger.Info("logger Init info")
	var err error
	_, args := stub.GetFunctionAndParameters()
	if len(args) != 1 {
		return shim.Error("Incorrect number of arguments. Expecting 1")
	}
	err = stub.PutState("value", []byte(args[0]))
	if err != nil {
		return shim.Error(err.Error())
	}
	return shim.Success(nil)
}

func (t *SimpleChaincode) Invoke(stub shim.ChaincodeStubInterface) pb.Response {
	logger.Info("logger Invoke")
	function, args := stub.GetFunctionAndParameters()
	switch function {
	case "get":
		return t.get(stub, args)
	case "set":
		return t.set(stub, args)
	default:
		return shim.Error("Not (yet) implemented")
	}
}

func (t *SimpleChaincode) get(stub shim.ChaincodeStubInterface, args []string) pb.Response {
	valuebytes, err := stub.GetState("value")
	if err != nil {
		return shim.Error("Error: could not retrieve value")
	}
	if valuebytes == nil {
		return shim.Error("Error: value nil")
	}
	return shim.Success(valuebytes)
}

func (t *SimpleChaincode) set(stub shim.ChaincodeStubInterface, args []string) pb.Response {
	logger.Info("logger set")
	var err error
	if len(args) != 1 {
		return shim.Error("Incorrect number of arguments. Expecting 1")
	}
	err = stub.PutState("value", []byte(args[0]))
	if err != nil {
		return shim.Error(err.Error())
	}
	return shim.Success(nil)
}

func main() {
	err := shim.Start(new(SimpleChaincode))
	if err != nil {
		fmt.Printf("Error starting Simple chaincode: %s", err)
	}

}
