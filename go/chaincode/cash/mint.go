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

func (t *K0Chaincode) mint(
	stub shim.ChaincodeStubInterface,
	args [][]byte,
) pb.Response {
	endpoint, is_set := os.LookupEnv("VERIFIER_ENDPOINT")
	if !is_set {
		endpoint = "http://verifier/"
	}
	k, err := util.VariableToFixed32(args[0])
	v, err := util.VariableToFixed8(args[1])
	cm, err := util.VariableToFixed32(args[2])
	newRoot, err := util.VariableToFixed32(args[3])
	k_elems, err := serverclient.Pack256Bits(endpoint, k)
	cm_elems, err := serverclient.Pack256Bits(endpoint, cm)
	if err != nil {
		msg := "Conversion error: %s"
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
