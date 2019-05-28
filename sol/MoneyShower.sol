pragma solidity ^0.5.3;

import "openzeppelin-solidity/contracts/math/SafeMath.sol";
import "openzeppelin-solidity/contracts/token/ERC20/IERC20.sol";

contract MoneyShower {
    using SafeMath for uint256;

    function transfer(
            address tokenContractAddress,
            address[] memory addresses,
            uint256[] memory amounts) public payable {
        require(addresses.length == amounts.length,
                "Number of addresses differs from number of amounts");
        IERC20 tokenContract = IERC20(tokenContractAddress);
        for (uint i = 0; i < addresses.length; i++) {
            tokenContract.transferFrom(msg.sender, addresses[i], amounts[i]);
        }
    }
}
