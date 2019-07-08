package main

import (
	"encoding/json"
	"fmt"
	"math/big"
	"os"

	"github.com/appliedblockchain/k0/go/data"
	"github.com/appliedblockchain/k0/go/serverclient"
	"github.com/appliedblockchain/k0/go/util"
	"github.com/hyperledger/fabric/core/chaincode/lib/cid"
	"github.com/hyperledger/fabric/core/chaincode/shim"
	pb "github.com/hyperledger/fabric/protos/peer"
)

func (t *K0Chaincode) checkIdentity(
	stub shim.ChaincodeStubInterface,
	expectedOrg []byte) (error, cid.ClientIdentity) {

	id, err := cid.New(stub)
	if err != nil {
		return err, nil
	}

	mspid, err := id.GetMSPID()
	if err != nil {
		return err, nil
	}

	if mspid != string(expectedOrg) {
		err := fmt.Errorf("Expected identity to be %s, was %s", string(expectedOrg), mspid)
		return err, id
	}

	return nil, id
}

func (t *K0Chaincode) mint(
	stub shim.ChaincodeStubInterface,
	args [][]byte,
) pb.Response {
	fmt.Println("BEGIN MINT ENDORSEMENT")
	expectedArgCount := 7
	if len(args) != expectedArgCount {
		str := "Incorrect number of arguments. Expecting %d, got: %d"
		return shim.Error(fmt.Sprintf(str, expectedArgCount, len(args)))
	}

	minterID, err := stub.GetState("minterID")

	if err != nil {
		return shim.Error(err.Error())
	}

	if len(minterID) == 0 {
		return shim.Error("minterID is not properly set, minting operation not permitted.")
	}

	err, _ = t.checkIdentity(stub, minterID)

	if err != nil {
		return shim.Error(err.Error())
	}

	endpoint, is_set := os.LookupEnv("VERIFIER_ENDPOINT")
	if !is_set {
		endpoint = "http://verifier/"
	}
	k, err := util.VariableToFixed32(args[0])
	v, err := util.VariableToFixed8(args[1])
	cm, err := util.VariableToFixed32(args[2])
	noteData, err := util.VariableToFixed136(args[3])
	newRoot, err := util.VariableToFixed32(args[4])
	k_elems, err := serverclient.Pack256Bits(endpoint, k)
	cm_elems, err := serverclient.Pack256Bits(endpoint, cm)
	if err != nil {
		msg := "Conversion error: %s"
		return shim.Error(fmt.Sprintf(msg, err.Error()))
	}
	var commitmentProof data.ProofJacobian
	err = json.Unmarshal(args[5], &commitmentProof)

	var additionProof data.ProofJacobian
	err = json.Unmarshal(args[6], &additionProof)

	publicInputsCommitment := []*big.Int{
		k_elems[0],
		k_elems[1],
		new(big.Int).SetUint64(util.BytesToUint8(v)),
		cm_elems[0],
		cm_elems[1],
	}
	verified, err := serverclient.Verify(
		endpoint, "commitment", commitmentProof, publicInputsCommitment)
	if err != nil {
		msg := "While trying to verify commitment proof: %s"
		return shim.Error(fmt.Sprintf(msg, err.Error()))
	}
	if !verified {
		return shim.Error("Commitment proof not verified")
	}

	currentRoot, err := stub.GetState("root")

	currentRootFixed, err := util.VariableToFixed32(currentRoot)

	logger.Infof("Root After Mint to %s.", util.BytesToHex32(currentRootFixed))

	numLeaves, err := stub.GetState("numLeaves")
	prev_root_elems, err := serverclient.Pack256Bits(endpoint, currentRootFixed)
	new_root_elems, err := serverclient.Pack256Bits(endpoint, newRoot)
	numLeavesFixed8, err := util.VariableToFixed8(numLeaves)
	numLeavesUint := util.BytesToUint8(numLeavesFixed8)

	fmt.Printf("Leaves count before mint: %+v \n", numLeavesUint)

	publicInputsAddition := []*big.Int{
		prev_root_elems[0],
		prev_root_elems[1],
		new(big.Int).SetUint64(numLeavesUint),
		cm_elems[0],
		cm_elems[1],
		new_root_elems[0],
		new_root_elems[1],
	}

	fmt.Printf("%+v \n", publicInputsAddition)

	verified, err = serverclient.Verify(
		endpoint, "addition", additionProof, publicInputsAddition)
	if err != nil {
		msg := "While trying to verify addition proof: %s"
		return shim.Error(fmt.Sprintf(msg, err.Error()))
	}
	if !verified {
		return shim.Error("Addition proof not verified")
	}

	err = stub.PutState("root", newRoot[:])
	if err != nil {
		return shim.Error(err.Error())
	}
	logger.Infof("Setting root to %s.", util.BytesToHex32(newRoot))
	eventVals := util.ConcatByteSlices([][]byte{
		cm[:],
		noteData[:],
		newRoot[:],
	})
	err = stub.SetEvent("Mint", eventVals)
	if err != nil {
		return shim.Error(err.Error())
	}

	numLeavesUint = numLeavesUint + 1
	newNumLeaves, err := util.UintToBytes8(numLeavesUint)

	if err != nil {
		msg := "Conversion for uint to bytes failed for newNumLeaves"
		return shim.Error(fmt.Sprintf(msg, err.Error()))
	}

	fmt.Printf("Leaves count after mint:\n uint: %+v\nbytes: %+v \n", numLeavesUint, newNumLeaves)

	stub.PutState("numLeaves", newNumLeaves[:])

	fmt.Println("MINT ENDORSEMENT SUCCESS")

	return shim.Success(nil)
}
