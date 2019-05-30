package util

import "math/big"

func StringToBigInt(input string) *big.Int {
	i := big.NewInt(0)
	i.SetString(input, 10)
	return i
}

func BigIntToString(input *big.Int) string {
	return input.Text(10)
}

func StringsToBigInts(inputs []string) []*big.Int {
	return MapStringsToBigInts(inputs, StringToBigInt)
}

func BigIntsToStrings(inputs []*big.Int) []string {
	return MapBigIntsToStrings(inputs, BigIntToString)
}
