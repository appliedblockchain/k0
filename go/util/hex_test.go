package util

import (
	"testing"
)

var testBytes = [32]byte{
	0, 255, 0, 255, 0, 255, 0, 255,
	0, 255, 0, 255, 0, 255, 0, 255,
	0, 255, 0, 255, 0, 255, 0, 255,
	0, 255, 0, 255, 0, 255, 0, 255,
}

var testHex = "0x" +
	"00ff00ff00ff00ff" +
	"00ff00ff00ff00ff" +
	"00ff00ff00ff00ff" +
	"00ff00ff00ff00ff"

func TestBytesToHex32(t *testing.T) {
	output := BytesToHex32(testBytes)
	if output != testHex {
		t.Errorf("Output is %s, expected %s", output, testHex)
	}
}

func TestHexToBytes32(t *testing.T) {
	output, err := HexToBytes32(testHex)
	if err != nil {
		t.Errorf(err.Error())
	}
	if output != testBytes {
		t.Errorf("Output is %s, expected %s", output, testBytes)
	}
}
