package data

type Fp2 struct {
	C0 string `json:"c0"`
	C1 string `json:"c1"`
}

type G1Affine struct {
	X string `json:"x"`
	Y string `json:"y"`
}

type G1Jacobian struct {
	X string `json:"x"`
	Y string `json:"y"`
	Z string `json:"z"`
}

type G2Affine struct {
	X Fp2 `json:"x"`
	Y Fp2 `json:"y"`
}

type G2Jacobian struct {
	X Fp2 `json:"x"`
	Y Fp2 `json:"y"`
	Z Fp2 `json:"z"`
}

type G1G1PairAffine struct {
	G G1Affine `json:"g"`
	H G1Affine `json:"h"`
}

type G1G1PairJacobian struct {
	G G1Jacobian `json:"g"`
	H G1Jacobian `json:"h"`
}

type G2G1PairAffine struct {
	G G2Affine `json:"g"`
	H G1Affine `json:"h"`
}

type G2G1PairJacobian struct {
	G G2Jacobian `json:"g"`
	H G1Jacobian `json:"h"`
}

type ProofAffine struct {
	G_A G1G1PairAffine `json:"g_A"`
	G_B G2G1PairAffine `json:"g_B"`
	G_C G1G1PairAffine `json:"g_C"`
	G_H G1Affine       `json:"g_H"`
	G_K G1Affine       `json:"g_K"`
}

type ProofJacobian struct {
	G_A G1G1PairJacobian `json:"g_A"`
	G_B G2G1PairJacobian `json:"g_B"`
	G_C G1G1PairJacobian `json:"g_C"`
	G_H G1Jacobian       `json:"g_H"`
	G_K G1Jacobian       `json:"g_K"`
}
