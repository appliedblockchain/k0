pragma solidity ^0.5.3;

import "DepositVerifier.sol";
import "WithdrawalVerifier.sol";
import "openzeppelin-solidity/contracts/token/ERC20/IERC20.sol";

contract MVPPT {
    uint[2] public root;
    uint public num_leaves = 0;
    DepositVerifier depositVerifier;
    WithdrawalVerifier withdrawalVerifier;
    IERC20 tokenContract;

    event Log(string);
    event Log(uint);
    event Log(uint, uint);
    event PrimaryInput(uint, uint, uint, uint, uint, uint, uint, uint, uint, uint);

    mapping(bytes32 => bool) snUsed;

    constructor(address tokenContractAddress,
            address depositVerifierAddress,
            address withdrawalVerifierAddress,
            uint[2] memory initialRoot) public {
        tokenContract = IERC20(tokenContractAddress);
        depositVerifier = DepositVerifier(depositVerifierAddress);
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
        uint[] memory inputs = new uint[](10);
        emit Log(num_leaves);
        emit Log(root[0], root[1]);
        inputs[0] = root[0];
        inputs[1] = root[1];
        inputs[2] = num_leaves;
        inputs[3] = comm_k[0];
        inputs[4] = comm_k[1];
        inputs[5] = v;
        inputs[6] = comm_cm[0];
        inputs[7] = comm_cm[1];
        inputs[8] = new_root[0];
        inputs[9] = new_root[1];
        emit PrimaryInput(inputs[0],inputs[1],inputs[2],inputs[3],inputs[4],inputs[5],inputs[6],inputs[7],inputs[8],inputs[9]);
        if (depositVerifier.verifyProof(
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
            root = new_root;
            num_leaves++;
        } else {
            emit Log(0xdead,0xbeef);
            // revert();
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