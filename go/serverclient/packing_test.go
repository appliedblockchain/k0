package serverclient

import (
	"github.com/appliedblockchain/zktrading/go/util"
	"testing"
)

func TestPacking(t *testing.T) {
	endpoint := "http://localhost:11400"
	hexVal, err := util.RandomBytes32()
	if err != nil {
		t.Fatalf(err.Error())
	}
	packed, err := Pack256Bits(endpoint, hexVal)
	if err != nil {
		t.Fatalf(err.Error())
	}
	unpacked, err := Unpack256Bits(endpoint, packed)
	if err != nil {
		t.Fatalf(err.Error())
	}
	if unpacked != hexVal {
		t.Fatalf("Unpacked packed not equal to input")
	}
}
