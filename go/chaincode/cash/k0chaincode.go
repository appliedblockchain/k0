package main

import (
	"encoding/json"
	"fmt"
	"github.com/appliedblockchain/zktrading/go/data"
	"github.com/appliedblockchain/zktrading/go/serverclient"
	"github.com/appliedblockchain/zktrading/go/util"
	"github.com/hyperledger/fabric/core/chaincode/shim"
	pb "github.com/hyperledger/fabric/protos/peer"
	"math/big"
	"os"
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

func (t *K0Chaincode) mint(
	stub shim.ChaincodeStubInterface,
	args [][]byte,
) pb.Response {
	endpoint, is_set := os.LookupEnv("VERIFIER_ENDPOINT")
	if !is_set {
		endpoint = "http://verifier/"
	}
	k, err := util.VariableToFixed32(args[0])
	if err != nil {
		msg := "While trying to convert variable length byte array " +
			"for k into fixed-length byte array: %s"
		return shim.Error(fmt.Sprintf(msg, err.Error()))
	}
	v, err := util.VariableToFixed8(args[1])
	if err != nil {
		msg := "While trying to convert variable length byte array " +
			"for v into fixed-length byte array: %s"
		return shim.Error(fmt.Sprintf(msg, err.Error()))
	}
	cm, err := util.VariableToFixed32(args[2])
	if err != nil {
		msg := "While trying to convert variable length byte array " +
			"for cm into fixed-length byte array: %s"
		return shim.Error(fmt.Sprintf(msg, err.Error()))
	}

	newRoot, err := util.VariableToFixed32(args[3])
	if err != nil {
		msg := "While trying to convert variable length byte array " +
			"for new root into fixed-length byte array: %s"
		return shim.Error(fmt.Sprintf(msg, err.Error()))
	}

	k_elems, err := serverclient.Pack256Bits(endpoint, k)
	if err != nil {
		msg := "While trying to pack k value: %s"
		return shim.Error(fmt.Sprintf(msg, err.Error()))
	}
	cm_elems, err := serverclient.Pack256Bits(endpoint, cm)
	if err != nil {
		msg := "While trying to pack cm value: %s"
		return shim.Error(fmt.Sprintf(msg, err.Error()))
	}
	var commitmentProof data.ProofJacobian
	err = json.Unmarshal(args[4], &commitmentProof)

	var additionProof data.ProofJacobian
	err = json.Unmarshal(args[5], &additionProof)

	publicInputs := []*big.Int{
		k_elems[0],
		k_elems[1],
		new(big.Int).SetUint64(util.BytesToUint8(v)),
		cm_elems[0],
		cm_elems[1],
	}
	verified, err := serverclient.Verify(
		endpoint, "commitment", commitmentProof, publicInputs)
	if err != nil {
		msg := "While trying to verify commitment proof: %s"
		return shim.Error(fmt.Sprintf(msg, err.Error()))
	}
	if !verified {
		return shim.Error("Commitment proof not verified")
	}

	// TODO verify addition proof

	err = stub.PutState("root", newRoot[:])
	if err != nil {
		return shim.Error(err.Error())
	}
	logger.Infof("Set root to %s.", util.BytesToHex32(newRoot))

	err = stub.SetEvent("Mint", append(cm[:], newRoot[:]...))
	if err != nil {
		return shim.Error(err.Error())
	}

	return shim.Success(nil)
}

type transferInput struct {
	sn_in_0    string `json:"sn_in_0"`
	sn_in_1    string `json:"sn_in_1"`
	cm_out_0   string `json:"cm_out_0"`
	cm_out_1   string `json:"cm_out_1"`
	data_out_0 string `json:"data_out_0"`
	data_out_1 string `json:"data_out_1"`
	newRoot    string `json:"newRoot"`
}

func (t *K0Chaincode) transfer(
	stub shim.ChaincodeStubInterface, args [][]byte,
) pb.Response {
	logger.Info("TRANSFER!")
	var err error
	params := transferInput{}
	err = json.Unmarshal(args[0], &params)
	if err != nil {
		return shim.Error(err.Error())
	}

	err = stub.PutState("root", []byte(params.newRoot))
	if err != nil {
		return shim.Error(err.Error())
	}
	logger.Infof("Set root to %s.", string(params.newRoot))

	eventVals := []string{
		params.sn_in_0, params.sn_in_1, params.cm_out_0,
		params.cm_out_1, params.data_out_0, params.data_out_1,
		params.newRoot}
	logger.Infof("eventVals %+v", eventVals)
	eventValsJson, err := json.Marshal(eventVals)
	if err != nil {
		return shim.Error(err.Error())
	}

	logger.Infof("eventvalsjson %+v", eventValsJson)
	err = stub.SetEvent("Transfer", []byte(eventValsJson))
	if err != nil {
		return shim.Error(err.Error())
	}

	return shim.Success(nil)
}
