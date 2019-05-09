package main

import (
	"bytes"
	"encoding/json"
	"fmt"
	"github.com/hyperledger/fabric/core/chaincode/shim"
	pb "github.com/hyperledger/fabric/protos/peer"
	"io/ioutil"
	"net/http"
)

type K0Chaincode struct {
}

type HashRequest struct {
	Jsonrpc string   `json:"jsonrpc"`
	Method  string   `json:"method"`
	Params  []string `json:"params"`
	Id      int      `json:"id"`
}

func (t *K0Chaincode) Init(stub shim.ChaincodeStubInterface) pb.Response {
	var err error
	_, args := stub.GetFunctionAndParameters()
	if len(args) != 1 {
		print("len")
		print(len(args))
		return shim.Error("Incorrect number of arguments. Expecting 1, got: " + len(args))
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
	case "mint":
		return t.mint(stub, args)
	case "transfer":
		return t.transfer(stub, args)
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

func (t *K0Chaincode) mint(stub shim.ChaincodeStubInterface, args []string) pb.Response {
	var logger = shim.NewLogger("mint")
	logger.Infof("MINT %s", string(args[0]))
	var err error
	cm := args[0]
	newRoot := args[1]
	err = stub.PutState("root", []byte(newRoot))
	if err != nil {
		return shim.Error(err.Error())
	}
	logger.Infof("Set root to %s.", string(newRoot))

	eventVals := []string{cm, newRoot}
	eventValsJson, err := json.Marshal(eventVals)
	if err != nil {
		return shim.Error(err.Error())
	}

	err = stub.SetEvent("Mint", []byte(eventValsJson))
	if err != nil {
		return shim.Error(err.Error())
	}

	return shim.Success(nil)
}

func (t *K0Chaincode) transfer(stub shim.ChaincodeStubInterface, args []string) pb.Response {
	var logger = shim.NewLogger("transfer")
	var err error
	sn_in_0 := args[0]
	sn_in_1 := args[1]
	cm_out_0 := args[2]
	cm_out_1 := args[3]
	data_out_0 := args[4]
	data_out_1 := args[5]
	newRoot := args[6]

	err = stub.PutState("root", []byte(newRoot))
	if err != nil {
		return shim.Error(err.Error())
	}
	logger.Infof("Set root to %s.", string(newRoot))

	eventVals := []string{sn_in_0, sn_in_1, cm_out_0, cm_out_1, data_out_0, data_out_1, newRoot}
	eventValsJson, err := json.Marshal(eventVals)
	if err != nil {
		return shim.Error(err.Error())
	}

	err = stub.SetEvent("Transfer", []byte(eventValsJson))
	if err != nil {
		return shim.Error(err.Error())
	}

	return shim.Success(nil)
}

func (t *K0Chaincode) set(stub shim.ChaincodeStubInterface, args []string) pb.Response {
	var logger = shim.NewLogger("ping")
	logger.Infof("SET %s", string(args[0]))
	if len(args) != 1 {
		return shim.Error("Incorrect number of arguments. Expecting 1")
	}

	// hashInput := HashInput{}
	// hashInput.left =  := json.Marshal(hashInput)
	// if err != nil {
	// 	logger.Error("JSON marshalling error: " + err.Error())
	// 	return shim.Error("Data error")
	// }
	// fmt.Printf(hashInputJson)

	hashRequest := HashRequest{}
	hashRequest.Jsonrpc = "2.0"
	hashRequest.Method = "hash"
	hashRequest.Id = 1
	hashRequest.Params = []string{
		"0x2f5a9c1f4a98922fc614748ff38f3ab0c00c52f108718cd55bd192ee6428c990",
		"0x2f5a9c1f4a98922fc614748ff38f3ab0c00c52f108718cd55bd192ee6428c990"}

	jsonValue, _ := json.Marshal(hashRequest)
	fmt.Printf("YOYOYO")
	fmt.Printf("%s", jsonValue)

	url := "http://verifier:4000/"
	resp, err := http.Post(url, "application/json", bytes.NewBuffer(jsonValue))
	if err != nil {
		panic(err)
	}

	fmt.Println("response Status:", resp.Status)
	fmt.Println("response Headers:", resp.Header)
	body, _ := ioutil.ReadAll(resp.Body)
	fmt.Println("response Body:", string(body))

	fmt.Printf("Setting value to: %s", args[0])
	err = stub.PutState("value", []byte("staticcc"))
	if err != nil {
		return shim.Error(err.Error())
	}

	evErr := stub.SetEvent("SetEvent", []byte("hello"))
	if evErr != nil {
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
