package util

import (
	"crypto/rand"
	"math/big"
	"testing"
)

func TestBigIntStringConversion(t *testing.T) {
	max := new(big.Int)
	max.Exp(big.NewInt(2), big.NewInt(128), nil).Sub(max, big.NewInt(1))
	nIn, err := rand.Int(rand.Reader, max)
	if err != nil {
		t.Fatalf(err.Error())
	}
	s := BigIntToString(nIn)
	nOut := StringToBigInt(s)
	if nOut.Cmp(nIn) != 0 {
		t.Fatalf("Output value was %s, expected %s", nIn, nOut)
	}
}
