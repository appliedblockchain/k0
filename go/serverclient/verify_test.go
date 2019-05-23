package k0chaincode_test

import (
	"encoding/json"
	"github.com/appliedblockchain/zktrading/fabric/chaincode/cash/verification"
	"github.com/appliedblockchain/zktrading/go/data"
	"io/ioutil"
	"testing"
)

func TestVerify(t *testing.T) {
	proofData, _ := ioutil.ReadFile("testdata/jacobian_proof.json")
	proof := data.ProofJacobian{}
	json.Unmarshal(proofData, &proof)
	inputsData, _ := ioutil.ReadFile("testdata/public_inputs.json")
	publicInputs := []string{}
	json.Unmarshal(inputsData, &publicInputs)

	result, err := verification.Verify("transfer", proof, publicInputs)
	if err != nil {
		t.Fatalf(err.Error())
	}
	if result != true {
		t.Fatalf("Verification failed")
	}
}
