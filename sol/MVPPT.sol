pragma solidity ^0.5.3;

import "AdditionVerifier.sol";
import "CommitmentVerifier.sol";
import "WithdrawalVerifier.sol";
import "openzeppelin-solidity/contracts/token/ERC20/IERC20.sol";

contract MVPPT {
    uint[2] public root;
    uint public num_leaves = 0;
    CommitmentVerifier commitmentVerifier;
    AdditionVerifier additionVerifier;
    WithdrawalVerifier withdrawalVerifier;
    IERC20 tokenContract;

    event Log(string);
    event Log(uint);
    event Log(uint, uint);
    event PrimaryInput(uint, uint, uint, uint, uint, uint, uint, uint, uint, uint);
    event PrimaryInput(uint, uint, uint, uint, uint, uint, uint);

    mapping(bytes32 => bool) snUsed;

    constructor(address tokenContractAddress,
        address commitmentVerifierAddress,
        address additionVerifierAddress,
        address withdrawalVerifierAddress,
        uint[2] memory initialRoot) public {
        tokenContract = IERC20(tokenContractAddress);
        commitmentVerifier = CommitmentVerifier(commitmentVerifierAddress);
        additionVerifier = AdditionVerifier(additionVerifierAddress);
        withdrawalVerifier = WithdrawalVerifier(withdrawalVerifierAddress);
        root = initialRoot;
    }

    // TODO add address as a sanity check
    function deposit(
        uint v,
        uint[2] memory comm_k,
        uint[2] memory comm_cm,
        uint[2] memory new_root,
        uint[18] memory commitmentProof,
        uint[18] memory additionProof

    ) public {

        require(
            tokenContract.transferFrom(msg.sender, address(this), v),
            "ERC20 transfer failed (sufficient allowance?)"
        );
        uint[] memory commInputs = new uint[](5);
        commInputs[0] = comm_k[0];
        commInputs[1] = comm_k[1];
        commInputs[2] = v;
        commInputs[3] = comm_cm[0];
        commInputs[4] = comm_cm[1];

        uint[] memory addInputs = new uint[](7);
        addInputs[0] = root[0];
        addInputs[1] = root[1];
        addInputs[2] = num_leaves;
        addInputs[3] = comm_cm[0];
        addInputs[4] = comm_cm[1];
        addInputs[5] = new_root[0];
        addInputs[6] = new_root[1];
        emit PrimaryInput(addInputs[0], addInputs[1], addInputs[2], addInputs[3], addInputs[4], addInputs[5], addInputs[6]);

        if (
            commitmentVerifier.verifyProof(
                [commitmentProof[0], commitmentProof[1]], // a
                [commitmentProof[2], commitmentProof[3]], // a_p
                [
                [commitmentProof[4], commitmentProof[5]], // b (0)
                [commitmentProof[6], commitmentProof[7]]  // b (1)
                ],
                [commitmentProof[8], commitmentProof[9]], // b_p
                [commitmentProof[10], commitmentProof[11]], // c
                [commitmentProof[12], commitmentProof[13]], // c_p
                [commitmentProof[14], commitmentProof[15]], // h
                [commitmentProof[16], commitmentProof[17]], // k
                commInputs)
            &&
            additionVerifier.verifyProof(
                [additionProof[0], additionProof[1]], // a
                [additionProof[2], additionProof[3]], // a_p
                [
                [additionProof[4], additionProof[5]], // b (0)
                [additionProof[6], additionProof[7]]  // b (1)
                ],
                [additionProof[8], additionProof[9]], // b_p
                [additionProof[10], additionProof[11]], // c
                [additionProof[12], additionProof[13]], // c_p
                [additionProof[14], additionProof[15]], // h
                [additionProof[16], additionProof[17]], // k
                addInputs)
        ) {
            root = new_root;
            num_leaves++;
        } else {
            emit Log(0xdead, 0xbeef);
            revert();
        }
    }

    function withdraw(
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
        bytes32 snHash = keccak256(abi.encode(sn));
        require(!snUsed[snHash], "sn has already been used.");
        snUsed[snHash] = true;
        uint[] memory inputs = new uint[](4);
        inputs[0] = root[0];
        inputs[1] = root[1];
        inputs[2] = sn[0];
        inputs[3] = sn[1];
        if (withdrawalVerifier.verifyProof(
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
            tokenContract.transfer(msg.sender, 1 ether);
        } else {
            revert();
        }
    }
}