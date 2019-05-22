package main

type fp2 struct {
	C0 string `json:"c0"`
	C1 string `json:"c1"`
}

type g1Jacobian struct {
	X string `json:"x"`
	Y string `json:"y"`
	Z string `json:"z"`
}

type g2Jacobian struct {
	X fp2 `json:"x"`
	Y fp2 `json:"y"`
	Z fp2 `json:"z"`
}

type g1g1PairJacobian struct {
	G g1Jacobian `json:"g"`
	H g1Jacobian `json:"h"`
}

type g2g1PairJacobian struct {
	G g2Jacobian `json:"g"`
	H g1Jacobian `json:"h"`
}

type proofJacobian struct {
	G_A g1g1PairJacobian `json:"g_A"`
	G_B g2g1PairJacobian `json:"g_B"`
	G_C g1g1PairJacobian `json:"g_C"`
	G_H g1Jacobian       `json:"g_H"`
	G_K g1Jacobian       `json:"g_K"`
}
