package main

import (
	"bytes"
	"encoding/json"
	"io/ioutil"
	"net/http"
)

type jsonRpcRequest struct {
	Id      string        `json:"id"`
	Jsonrpc string        `json:"jsonrpc"`
	Method  string        `json:"method"`
	Params  []interface{} `json:"params"`
}

type jsonRpcVerificationResponse struct {
	Id      string `json:"id"`
	Jsonrpc string `json:"jsonrpc"`
	Result  bool   `json:"result"`
}

func callJsonRpc(endpoint string, method string, params []interface{}) ([]byte, error) {
	request := jsonRpcRequest{}
	request.Jsonrpc = "2.0"
	request.Method = method
	request.Params = params
	request.Id = "1337"

	jsonValue, err := json.Marshal(request)
	if err != nil {
		return nil, err
	}

	resp, err := http.Post(endpoint, "application/json", bytes.NewBuffer(jsonValue))
	if err != nil {
		return nil, err
	}

	defer resp.Body.Close()
	return ioutil.ReadAll(resp.Body)
}

func Verify(proofType string, proof proofJacobian, publicInputs []string) (bool, error) {
	body, err := callJsonRpc("http://localhost:11400/", "verifyProof", []interface{}{
		proofType,
		proof,
		publicInputs,
	})
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
