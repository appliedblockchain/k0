package main

import (
	"encoding/json"
	"fmt"
	"github.com/appliedblockchain/zktrading/go/data"
	"github.com/appliedblockchain/zktrading/go/serverclient"
	"github.com/appliedblockchain/zktrading/go/util"
	"github.com/hyperledger/fabric/core/chaincode/shim"
	"testing"
)

func setup(t *testing.T) *shim.MockStub {
	stub := shim.NewMockStub("mockStub", new(K0Chaincode))
	if stub == nil {
		t.Fatalf("MockStub creation failed")
	}
	params := [][]byte{[]byte("init"), []byte("foo")}
	response := stub.MockInit("100", params)
	if response.Status != shim.OK {
		t.Fatalf(response.Message)
	}
	return stub
}

func TestMint(t *testing.T) {
	stub := setup(t)
	a_sk, err := util.RandomBytes32()
	if err != nil {
		t.Fatalf(err.Error())
	}
	a_pk, err := serverclient.PrfAddr("http://localhost:11400", a_sk)
	if err != nil {
		t.Fatalf(err.Error())
	}
	rho, err := util.RandomBytes32()
	if err != nil {
		t.Fatalf(err.Error())
	}
	r, err := util.RandomBytes48()
	if err != nil {
		t.Fatalf(err.Error())
	}
	fmt.Printf("%+v", stub)
	fmt.Printf("%+v", a_pk)
	fmt.Printf("%+v", rho)
	fmt.Printf("%+v", r)
	v := uint64(4000)
	res, err := serverclient.DepositCommitmentProof(
		"http://localhost:11400", a_pk, rho, r, v)
	if err != nil {
		t.Fatalf(err.Error())
	}

	commitmentProofJson, err := json.Marshal(res.ProofJacobian)
	if err != nil {
		t.Fatalf(err.Error())
	}

	v_bytes, err := util.UintToBytes8(v)
	if err != nil {
		t.Fatalf(err.Error())
	}

	// TODO Test with MT server
	newRoot, err := util.RandomBytes32()
	if err != nil {
		t.Fatalf(err.Error())
	}

	// TODO Test with actual proof
	var additionProof data.ProofJacobian
	additionProofJson, err := json.Marshal(additionProof)
	if err != nil {
		t.Fatalf(err.Error())
	}

	response := stub.MockInvoke("200", [][]byte{
		[]byte("mint"),
		res.K[:],
		v_bytes[:],
		res.CM[:],
		newRoot[:],
		commitmentProofJson,
		additionProofJson,
	})
	if response.Status == shim.ERROR {
		fmt.Printf("%+v", response)
		t.Fatalf("Mint failed")
	}
}
