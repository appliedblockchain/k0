#ifndef PRINTBITS_H
#define PRINTBITS_H

#include <bitset>
#include <iomanip>
#include <iostream>
#include <vector>
 
void printbits(std::vector<bool> bv) {
  uint wordlength = 32;
  std::bitset<32> word;
  std::cout << "0x";
  for (uint i = 0; i < bv.size(); i++) {
      word.set(wordlength - 1 - (i % wordlength), bv[i]);
      if (i % wordlength == wordlength - 1 || i == bv.size() - 1) {
        std::cout << std::setfill('0') << std::setw(8) << std::hex << word.to_ulong();
      }
  }
  std::cout << std::endl;
}

void printbits_vec(std::vector<std::vector<bool>> bvv) {
  for (uint i = 0; i < bvv.size(); i++) {
    printbits(bvv[i]);
  }
}

#endif // PRINTBITS_H