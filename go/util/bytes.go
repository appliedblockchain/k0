package util

import (
	"encoding/binary"
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

func VariableToFixed8(input []byte) ([8]byte, error) {
	if len(input) != 8 {
		msg := "Byte array is of length %d, expected 8"
		return [8]byte{}, fmt.Errorf(msg, len(input))
	}
	var result [8]byte
	copy(result[:], input[:8])
	return result, nil
}

func UintToBytes8(input uint64) ([8]byte, error) {
	b := make([]byte, 8)
	binary.BigEndian.PutUint64(b, input)
	return VariableToFixed8(b)
}

func BytesToUint8(input [8]byte) uint64 {
	return uint64(binary.BigEndian.Uint64(input[:]))
}
