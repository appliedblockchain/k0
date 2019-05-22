package main

import (
	"encoding/json"
	"io/ioutil"
	"testing"
)

func TestVerify(t *testing.T) {
	proofData, _ := ioutil.ReadFile("testdata/jacobian_proof.json")
	proof := proofJacobian{}
	json.Unmarshal(proofData, &proof)
	inputsData, _ := ioutil.ReadFile("testdata/public_inputs.json")
	publicInputs := []string{}
	json.Unmarshal(inputsData, &publicInputs)

	result, err := Verify("transfer", proof, publicInputs)
	if err != nil {
		t.Fatalf(err.Error())
	}
	if result != true {
		t.Fatalf("Verification failed")
	}
}
