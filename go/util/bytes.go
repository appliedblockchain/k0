package util

import (
	"fmt"
)

func VariableToFixed32(input []byte) ([32]byte, error) {
	if len(input) != 32 {
		msg := "Byte array is of length %d, expected 32"
		return [32]byte{}, fmt.Errorf(msg, len(input))
	}
	var result [32]byte
	copy(result[:], input[:32])
	return result, nil
}

func VariableToFixed48(input []byte) ([48]byte, error) {
	if len(input) != 48 {
		msg := "Byte array is of length %d, expected 48"
		return [48]byte{}, fmt.Errorf(msg, len(input))
	}
	var result [48]byte
	copy(result[:], input[:48])
	return result, nil
}
