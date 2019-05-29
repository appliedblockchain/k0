package util

import "math/big"

// https://gobyexample.com/collection-functions
func Map(vs []string, f func(string) string) []string {
	vsm := make([]string, len(vs))
	for i, v := range vs {
		vsm[i] = f(v)
	}
	return vsm
}

func MapStringsToBigInts(vs []string, f func(string) *big.Int) []*big.Int {
	vsm := make([]*big.Int, len(vs))
	for i, v := range vs {
		vsm[i] = f(v)
	}
	return vsm
}

func MapBigIntsToStrings(vs []*big.Int, f func(*big.Int) string) []string {
	vsm := make([]string, len(vs))
	for i, v := range vs {
		vsm[i] = f(v)
	}
	return vsm
}
