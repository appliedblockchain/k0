pragma solidity ^0.5.3;

import 'AdditionVerifier.sol';
import 'InclusionVerifier.sol';

contract MerkleTree {
    uint[2] public root;
    uint public num_leaves = 0;
    AdditionVerifier additionVerifier;
    InclusionVerifier inclusionVerifier;

    event Log(string);

    constructor(address additionVerifierAddress, address inclusionVerifierAddress, uint[2] memory initialRoot) public {
        additionVerifier = AdditionVerifier(additionVerifierAddress);
        inclusionVerifier = InclusionVerifier(inclusionVerifierAddress);
        root = initialRoot;
    }

    function add(
        uint[2] memory leaf,
        uint[2] memory newRoot,
        uint[2] memory a,
        uint[2] memory a_p,
        uint[2][2] memory b,
        uint[2] memory b_p,
        uint[2] memory c,
        uint[2] memory c_p,
        uint[2] memory h,
        uint[2] memory k
    ) public {
        uint[] memory inputs = new uint[](7);
        inputs[0] = num_leaves;
        inputs[1] = root[0];
        inputs[2] = root[1];
        inputs[3] = leaf[0];
        inputs[4] = leaf[1];
        inputs[5] = newRoot[0];
        inputs[6] = newRoot[1];
        if (additionVerifier.verifyProof(
            a, 
            a_p, 
            b,
            b_p,
            c,
            c_p,
            h,
            k,
            inputs
        )) {
            root = newRoot;
            num_leaves++;
        } else {
            revert();
        }
    }

    function verifyKnowledgeOfLeafSecrets(
        uint[2] memory sn,
        uint[2] memory a,
        uint[2] memory a_p,
        uint[2][2] memory b,
        uint[2] memory b_p,
        uint[2] memory c,
        uint[2] memory c_p,
        uint[2] memory h,
        uint[2] memory k
    ) public {
        uint[] memory inputs = new uint[](4);
        inputs[0] = root[0];
        inputs[1] = root[1];
        inputs[2] = sn[0];
        inputs[3] = sn[1];
        if (inclusionVerifier.verifyProof(
            a, 
            a_p, 
            b,
            b_p,
            c,
            c_p,
            h,
            k,
            inputs
        )) {
            // noop
        } else {
            revert();
        }
    }
}