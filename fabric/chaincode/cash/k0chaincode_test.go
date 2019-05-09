package main

import (
	"fmt"
	"github.com/hyperledger/fabric/core/chaincode/shim"
	"testing"
)

func TestAbs(t *testing.T) {

	fmt.Println("Entering Test")

	stub := shim.NewMockStub("mockStub", new(K0Chaincode))
	if stub == nil {
		t.Fatalf("MockStub creation failed")
	}

	result := stub.MockInvoke("001",
		[][]byte{[]byte("mint"),
			[]byte("do"),
			[]byte("re"),
			[]byte("mi"),
		})

	if result.Status != shim.OK {
		t.Errorf("Didn't work")
	}
}
