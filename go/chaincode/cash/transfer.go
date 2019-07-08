package main

import (
	"fmt"

	"github.com/appliedblockchain/k0/go/util"
	"github.com/hyperledger/fabric/core/chaincode/shim"
	pb "github.com/hyperledger/fabric/protos/peer"
)

func (t *K0Chaincode) transfer(
	stub shim.ChaincodeStubInterface, args [][]byte,
) pb.Response {
	fmt.Println("BEGIN TRANSFER ENDORSMENT")

	sn_in_0, err := util.VariableToFixed32(args[0])
	sn_in_1, err := util.VariableToFixed32(args[1])
	cm_out_0, err := util.VariableToFixed32(args[2])
	cm_out_1, err := util.VariableToFixed32(args[3])
	data_out_0, err := util.VariableToFixed136(args[4])
	data_out_1, err := util.VariableToFixed136(args[5])
	newRoot, err := util.VariableToFixed32(args[6])
	if err != nil {
		msg := "Conversion error: %s"
		return shim.Error(fmt.Sprintf(msg, err.Error()))
	}

	err = stub.PutState("root", newRoot[:])
	if err != nil {
		return shim.Error(err.Error())
	}
	logger.Infof("Set root to %s.", util.BytesToHex32(newRoot))
	eventVals := util.ConcatByteSlices([][]byte{
		sn_in_0[:],
		sn_in_1[:],
		cm_out_0[:],
		cm_out_1[:],
		data_out_0[:],
		data_out_1[:],
		newRoot[:],
	})
	err = stub.SetEvent("Transfer", eventVals)
	if err != nil {
		return shim.Error(err.Error())
	}

	fmt.Println("TRANSFER ENDORSMENT SUCCESS")

	return shim.Success(nil)
}
