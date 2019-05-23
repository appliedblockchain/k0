package util

import (
	"crypto/rand"
)

func RandomBytes32() ([32]byte, error) {
	b := make([]byte, 32)
	_, err := rand.Read(b)
	if err != nil {
		return [32]byte{}, err
	}
	return VariableToFixed32(b)
}

func RandomBytes48() ([48]byte, error) {
	b := make([]byte, 48)
	_, err := rand.Read(b)
	if err != nil {
		return [48]byte{}, err
	}
	return VariableToFixed48(b)
}
