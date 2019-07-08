package serverclient

import (
	"encoding/json"
	"github.com/appliedblockchain/k0/go/jsonrpc"
	"github.com/appliedblockchain/k0/go/util"
)

type prfAddrJsonRpcResponse struct {
	Id      string `json:"id"`
	Jsonrpc string `json:"jsonrpc"`
	Result  string `json:"result"`
}

func PrfAddr(endpoint string, a_sk [32]byte) ([32]byte, error) {
	a_sk_hex := util.BytesToHex32(a_sk)
	body, err := jsonrpc.Call(endpoint, "prf_addr", []interface{}{a_sk_hex})
	if err != nil {
		return [32]byte{}, err
	}
	// TODO check for error in JSONRPC response
	res := prfAddrJsonRpcResponse{}
	json.Unmarshal(body, &res)
	if err != nil {
		return [32]byte{}, err
	}
	return util.HexToBytes32(res.Result)
}
