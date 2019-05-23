package serverclient

import (
	"encoding/json"
	"github.com/appliedblockchain/zktrading/go/data"
	"github.com/appliedblockchain/zktrading/go/jsonrpc"
	"math/big"
)

type jsonRpcVerificationResponse struct {
	Id      string `json:"id"`
	Jsonrpc string `json:"jsonrpc"`
	Result  bool   `json:"result"`
}

func Verify(
	endpoint string,
	proofType string,
	proof data.ProofJacobian,
	publicInputs []*big.Int,
) (bool, error) {
	body, err := jsonrpc.Call(
		endpoint,
		"verifyProof",
		[]interface{}{
			proofType,
			proof,
			publicInputs,
		},
	)
	if err != nil {
		return false, err
	}
	res := jsonRpcVerificationResponse{}
	json.Unmarshal(body, &res)
	if err != nil {
		return false, err
	}
	return res.Result, nil
}
