package main

import (
	"fmt"
	"github.com/hyperledger/fabric/core/chaincode/shim"
	pb "github.com/hyperledger/fabric/protos/peer"
)

type K0Chaincode struct {
}

func (t *K0Chaincode) Init(stub shim.ChaincodeStubInterface) pb.Response {
	var err error
	_, args := stub.GetFunctionAndParameters()
	if len(args) != 1 {
		return shim.Error("Incorrect number of arguments. Expecting 1")
	}
	fmt.Printf("Setting value to: %s", args[0])
	err = stub.PutState("value", []byte(args[0]))
	if err != nil {
		return shim.Error(err.Error())
	}
	return shim.Success(nil)
}

func (t *K0Chaincode) Invoke(stub shim.ChaincodeStubInterface) pb.Response {
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

func (t *K0Chaincode) get(stub shim.ChaincodeStubInterface, args []string) pb.Response {
	valuebytes, err := stub.GetState("value")
	if err != nil {
		return shim.Error("Error: could not retrieve value")
	}
	if valuebytes == nil {
		return shim.Error("Error: value nil")
	}
	return shim.Success(valuebytes)
}

func (t *K0Chaincode) set(stub shim.ChaincodeStubInterface, args []string) pb.Response {
	var err error
	if len(args) != 1 {
		return shim.Error("Incorrect number of arguments. Expecting 1")
	}
	fmt.Printf("Setting value to: %s", args[0])
	err = stub.PutState("value", []byte(args[0]))
	if err != nil {
		return shim.Error(err.Error())
	}
	return shim.Success(nil)
}

func main() {
	err := shim.Start(new(K0Chaincode))
	if err != nil {
		fmt.Printf("Error starting K0 chaincode: %s", err)
	}

}
