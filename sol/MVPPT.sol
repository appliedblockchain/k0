pragma solidity ^0.5.3;

import "CommitmentVerifier.sol";
import "WithdrawalVerifier.sol";
import "openzeppelin-solidity/contracts/token/ERC20/IERC20.sol";

contract MVPPT {
    uint[2] public root;
    uint public num_leaves = 0;
    CommitmentVerifier commitmentVerifier;
    WithdrawalVerifier withdrawalVerifier;
    IERC20 tokenContract;

    event Log(string);
    event Log(uint);
    event Log(uint, uint);
    event PrimaryInput(uint, uint, uint, uint, uint, uint, uint, uint, uint, uint);

    mapping(bytes32 => bool) snUsed;

    constructor(address tokenContractAddress,
            address commitmentVerifierAddress,
            address withdrawalVerifierAddress,
            uint[2] memory initialRoot) public {
        tokenContract = IERC20(tokenContractAddress);
        commitmentVerifier = CommitmentVerifier(commitmentVerifierAddress);
        withdrawalVerifier = WithdrawalVerifier(withdrawalVerifierAddress);
        root = initialRoot;
    }

    // TODO add address as a sanity check
    function deposit(
        uint v,
        uint[2] memory comm_k,
        uint[2] memory comm_cm,
        uint[2] memory new_root,
        uint[2] memory a,
        uint[2] memory a_p,
        uint[2][2] memory b,
        uint[2] memory b_p,
        uint[2] memory c,
        uint[2] memory c_p,
        uint[2] memory h,
        uint[2] memory k
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

        if (commitmentVerifier.verifyProof(
            a, 
            a_p, 
            b,
            b_p,
            c,
            c_p,
            h,
            k,
            commInputs
        )) {
            root = new_root;
            num_leaves++;
        } else {
            emit Log(0xdead,0xbeef);
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