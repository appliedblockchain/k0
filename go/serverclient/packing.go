package serverclient

import (
	"encoding/json"
	"github.com/appliedblockchain/k0/go/jsonrpc"
	"github.com/appliedblockchain/k0/go/util"
	"math/big"
)

type pack256BitsResponse struct {
	Id      string    `json:"id"`
	Jsonrpc string    `json:"jsonrpc"`
	Result  [2]string `json:"result"`
}

type unpack256BitsResponse struct {
	Id      string `json:"id"`
	Jsonrpc string `json:"jsonrpc"`
	Result  string `json:"result"`
}

func Pack256Bits(endpoint string, input [32]byte) ([2]*big.Int, error) {
	params := []interface{}{util.BytesToHex32(input)}
	body, err := jsonrpc.Call(endpoint, "pack256Bits", params)
	if err != nil {
		return [2]*big.Int{}, err
	}
	// TODO check for error in JSONRPC response
	res := pack256BitsResponse{}
	if err := json.Unmarshal(body, &res); err != nil {
		return [2]*big.Int{}, err
	}
	result := [2]*big.Int{big.NewInt(0), big.NewInt(0)}
	result[0].SetString(res.Result[0], 10)
	result[1].SetString(res.Result[1], 10)
	return result, nil
}

func Unpack256Bits(endpoint string, input [2]*big.Int) ([32]byte, error) {
	params := []interface{}{input[0].Text(10), input[1].Text(10)}
	body, err := jsonrpc.Call(endpoint, "unpack256Bits", params)
	// TODO check for error in JSONRPC response
	res := unpack256BitsResponse{}
	err = json.Unmarshal(body, &res)
	if err != nil {
		return [32]byte{}, err
	}
	return util.HexToBytes32(res.Result)
}
