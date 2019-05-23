package util

import (
	"encoding/hex"
	"fmt"
)

func BytesToHex32(input [32]byte) string {
	return "0x" + hex.EncodeToString(input[:])
}

func HexToBytes32(input string) ([32]byte, error) {
	if len(input) != 66 {
		msg := "Input to Hex2Buf is of length %d, expected 66"
		return [32]byte{}, fmt.Errorf(msg, len(input))
	}
	if input[:2] != "0x" {
		err := fmt.Errorf("Input to Hex2Buf not prefixed with \"0x\"")
		return [32]byte{}, err
	}
	decoded, err := hex.DecodeString(input[2:])
	if err != nil {
		return [32]byte{}, err
	}
	return VariableToFixed32(decoded)
}

func BytesToHex48(input [48]byte) string {
	return "0x" + hex.EncodeToString(input[:])
}

func HexToBytes48(input string) ([48]byte, error) {
	if len(input) != 66 {
		msg := "Input to Hex2Buf is of length %d, expected 66"
		return [48]byte{}, fmt.Errorf(msg, len(input))
	}
	if input[:2] != "0x" {
		err := fmt.Errorf("Input to Hex2Buf not prefixed with \"0x\"")
		return [48]byte{}, err
	}
	decoded, err := hex.DecodeString(input[2:])
	if err != nil {
		return [48]byte{}, err
	}
	return VariableToFixed48(decoded)
}
