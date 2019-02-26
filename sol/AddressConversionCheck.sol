pragma solidity ^0.5.3;

contract AddressConversionCheck {

    function test(address input) public returns (uint) {
        return uint(input);
    }
}