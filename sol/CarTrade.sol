pragma solidity ^0.5.3;

import "CarToken.sol";
import "DollarCoin.sol";
import "openzeppelin-solidity/contracts/ownership/Ownable.sol";

contract CarTrade is Ownable {
    CarToken carToken;
    DollarCoin dollarCoin;
    uint256 tokenId;
    uint256 public askPrice;

    constructor(
        address _carTokenAddress,
        address _dollarCoinAddress,
        uint256 _tokenId,
        uint256 _askPrice) public {
        carToken = CarToken(_carTokenAddress);
        dollarCoin = DollarCoin(_dollarCoinAddress);
        tokenId = _tokenId;
        askPrice = _askPrice;
    }

    function buy() public {
        dollarCoin.transferFrom(msg.sender, address(this), askPrice);
        dollarCoin.transfer(owner(), askPrice);
        carToken.transferFrom(owner(), msg.sender, tokenId);
        address payable theOwner = address(uint160(owner()));
        selfdestruct(theOwner);
    }
}
