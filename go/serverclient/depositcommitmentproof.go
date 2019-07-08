package serverclient

import (
	"encoding/json"
	"fmt"
	"github.com/appliedblockchain/k0/go/data"
	"github.com/appliedblockchain/k0/go/jsonrpc"
	"github.com/appliedblockchain/k0/go/util"
	"strconv"
)

type depositCommitmentProofJsonRpcResult struct {
	CM            string             `json:"cm"`
	K             string             `json:"k"`
	ProofAffine   data.ProofAffine   `json:"proof_affine"`
	ProofJacobian data.ProofJacobian `json:"proof_jacobian"`
}

type depositCommitmentProofJsonRpcResponse struct {
	Id      string                              `json:"id"`
	Jsonrpc string                              `json:"jsonrpc"`
	Result  depositCommitmentProofJsonRpcResult `json:"result"`
}

type DepositCommitmentProofResponse struct {
	CM            [32]byte
	K             [32]byte
	ProofAffine   data.ProofAffine
	ProofJacobian data.ProofJacobian
}

func DepositCommitmentProof(
	endpoint string,
	a_pk [32]byte,
	rho [32]byte,
	r [48]byte,
	v uint64,
) (DepositCommitmentProofResponse, error) {
	a_pk_hex := util.BytesToHex32(a_pk)
	rho_hex := util.BytesToHex32(rho)
	r_hex := util.BytesToHex48(r)
	params := []interface{}{
		a_pk_hex,
		rho_hex,
		r_hex,
		strconv.FormatUint(v, 10),
	}
	body, err := jsonrpc.Call(endpoint, "depositCommitmentProof", params)
	if err != nil {
		msg := "Error in depositCommitmentProof JSNORPC call: %s"
		return DepositCommitmentProofResponse{}, fmt.Errorf(msg, err.Error())
	}
	res := depositCommitmentProofJsonRpcResponse{}
	json.Unmarshal(body, &res)
	if err != nil {
		msg := "When trying to unmarshal prf_addr JSONRPC response: %s"
		return DepositCommitmentProofResponse{},
			fmt.Errorf(msg, err.Error())
	}
	cm, err := util.HexToBytes32(res.Result.CM)
	if err != nil {
		msg := "When trying to convert cm hex to bytes: %s"
		return DepositCommitmentProofResponse{},
			fmt.Errorf(msg, err.Error())
	}
	k, err := util.HexToBytes32(res.Result.K)
	if err != nil {
		msg := "When trying to convert k hex to bytes: %s"
		return DepositCommitmentProofResponse{},
			fmt.Errorf(msg, err.Error())
	}
	return DepositCommitmentProofResponse{
		cm, k, res.Result.ProofAffine, res.Result.ProofJacobian,
	}, nil
}
