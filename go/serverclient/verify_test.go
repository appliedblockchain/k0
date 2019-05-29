package serverclient

import (
	"encoding/json"
	"github.com/appliedblockchain/zktrading/go/data"
	"github.com/appliedblockchain/zktrading/go/util"
	"io/ioutil"
	"os"
	"testing"
)

func TestVerify(t *testing.T) {
	endpoint, is_set := os.LookupEnv("VERIFIER_ENDPOINT")
	if !is_set {
		endpoint = "http://localhost:11400"
	}
	proofData, _ := ioutil.ReadFile("testdata/jacobian_proof.json")
	proof := data.ProofJacobian{}
	json.Unmarshal(proofData, &proof)
	inputsData, _ := ioutil.ReadFile("testdata/public_inputs.json")
	publicInputStrings := []string{}
	json.Unmarshal(inputsData, &publicInputStrings)
	publicInputs := util.StringsToBigInts(publicInputStrings)
	result, err := Verify(endpoint, "transfer", proof, publicInputs)
	if err != nil {
		t.Fatalf(err.Error())
	}
	if result != true {
		t.Fatalf("Verification failed")
	}
}
