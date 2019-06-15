pragma solidity ^0.5.3;

import "Callee.sol";
import "CarToken.sol";
import "MVPPT.sol";
import "openzeppelin-solidity/contracts/ownership/Ownable.sol";

contract HiddenPriceCarTrade is Ownable, Callee {
    CarToken carToken;
    MVPPT mvppt;
    uint256 tokenId;
    uint256 expectedCM_0;
    uint256 expectedCM_1;

    constructor(
        address _carTokenAddress,
        address _mvppt,
        uint256 _tokenId,
        uint256 _expectedCM_0,
        uint256 _expectedCM_1) public {
        carToken = CarToken(_carTokenAddress);
        tokenId = _tokenId;
        expectedCM_0 = _expectedCM_0;
        expectedCM_1 = _expectedCM_1;
    }

    function handle(uint data_0, uint data_1) public returns (bool) {
        if (data_0 == expectedCM_0 && data_1 == expectedCM_1) {
            carToken.transferFrom(owner(), tx.origin, tokenId);
        }
    }
}