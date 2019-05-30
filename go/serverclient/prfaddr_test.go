package serverclient

import (
	"github.com/appliedblockchain/zktrading/go/util"
	"testing"
)

func TestPrfAddr(t *testing.T) {
	a_sk, err := util.RandomBytes32()
	if err != nil {
		t.Fatalf(err.Error())
	}
	_, err = PrfAddr("http://localhost:11400", a_sk)
	if err != nil {
		t.Fatalf(err.Error())
	}
}
