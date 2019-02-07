#include <libff/algebra/curves/public_params.hpp>
#include <libsnark/common/default_types/r1cs_ppzksnark_pp.hpp>
#include <libsnark/gadgetlib1/protoboard.hpp>
#include "circuits/gadgets/prfs.hpp"
#include "crypto/sha256.h"

using namespace std;
using namespace libff;
using namespace libsnark;

int main()
{
    typedef libff::Fr<default_r1cs_ppzksnark_pp> FieldT;
    default_r1cs_ppzksnark_pp::init_public_params();
    
    protoboard<FieldT> pb;
    pb_variable<FieldT> ZERO;
    pb_variable_array<FieldT> x;
    pb_variable_array<FieldT> y;
    auto result = make_shared<digest_variable<FieldT>>(pb, 256, "result");
    
    ZERO.allocate(pb, "ZERO");
    x.allocate(pb, 256, "x");
    y.allocate(pb, 254, "y");
    
    PRF_gadget<FieldT> prfg{pb, ZERO, x, true, false, y, result};
    
    PRF_addr_gadget<FieldT> prf_addr{pb, ZERO, x, result};
    
    PRF_sn_gadget<FieldT> prf_sn{pb, ZERO, x, y, result};
    
    unsigned char blob[64];

    for (int i = 0; i < 64; i++) {
        blob[i] = 0;
    }
    cout << endl;
    cout << "blob";
    for (int i = 0; i < 64; i++) {
        cout << uint(blob[i]) << " ";
    }
    cout << endl;
    
    unsigned char res[32];
    CSHA256 hasher;
    hasher.Write(blob, 64);
    hasher.FinalizeNoPadding(res);
    for (int i = 0; i < 32; i++) {
        cout << hex << uint(res[i]) << " ";
    }
    cout << endl;
//
//    memcpy(&blob[0], x.begin(), 32);
//    memcpy(&blob[32], y.begin(), 32);
//
//    blob[0] &= 0x0F;
//    blob[0] |= (a ? 1 << 7 : 0) | (b ? 1 << 6 : 0) | (c ? 1 << 5 : 0) | (d ? 1 << 4 : 0);
//
//    CSHA256 hasher;
//    hasher.Write(blob, 64);
//    hasher.FinalizeNoPadding(res.begin());
    return 0;
}

