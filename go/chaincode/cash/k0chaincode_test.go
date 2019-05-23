package k0chaincode

import (
	"encoding/json"
	"fmt"
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
	println("Hello world")
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
	v := uint64(4000)
	res, err := serverclient.DepositCommitmentProof(
		"http://localhost:11400", a_pk, rho, r, v)
	if err != nil {
		t.Fatalf(err.Error())
	}

	proofJson, err := json.Marshal(res.ProofJacobian)

	response := stub.MockInvoke("200", [][]byte{
		[]byte("mint"),
		res.CM[:],
		res.K[:],
		proofJson,
	})
	if response.Status == shim.ERROR {
		fmt.Printf("%+v", response)
		t.Fatalf("Mint failed")
	}
}
