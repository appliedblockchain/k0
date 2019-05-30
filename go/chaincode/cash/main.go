package main

import (
	"fmt"
	"github.com/hyperledger/fabric/core/chaincode/shim"
)

func main() {
	err := shim.Start(new(K0Chaincode))
	if err != nil {
		fmt.Printf("Error starting K0 chaincode: %s", err)
	}

}
