#ifndef SERIALIZATION_H
#define SERIALIZATION_H

#include <algorithm>
#include <bitset>
#include <iomanip>
#include <libff/common/utils.hpp>
#include <libff/common/serialization.hpp>

using namespace std;

unsigned long hex2long(string hex_string) {
  unsigned long x;
  std::stringstream ss;
  ss << std::hex << hex_string;
  ss >> x;
  return x;
}

vector<bool> hex2bits(string input) {
  if (input.length() == 0) {
    throw invalid_argument("Input string is empty");
  }
  input = input.substr(2);
  if (!(input.length() % 8 == 0)) {
    throw invalid_argument("Input string (" + input + ") is of length " + to_string(input.length()));
  }
  vector<bool> input_as_bits;
  for (uint i = input.length(); i >= 8; i = i - 8)
  {
    unsigned long num(hex2long(input.substr(i - 8, 8)));
    bitset<(size_t)32> bs(num);
    for (uint j = 0; j < bs.size(); j++)
    {
      input_as_bits.push_back(bs[j]);
    }
  }
  reverse(input_as_bits.begin(), input_as_bits.end());
  return input_as_bits;
}

std::vector<std::vector<bool>> hex2bitsVec(std::vector<std::string> input) {
  std::vector<std::vector<bool>> output(input.size());
  std::transform(input.begin(), input.end(), output.begin(), hex2bits);
  return output;
}

std::vector<std::vector<std::vector<bool>>> hex2bitsVecVec(std::vector<std::vector<std::string>> input) {
  std::vector<std::vector<std::vector<bool>>> output(input.size());
  std::transform(input.begin(), input.end(), output.begin(), hex2bitsVec);
  return output;
}

std::string bits2hex(std::vector<bool> bv) {
  std::stringstream str_stream;
  std::bitset<32> word;
  str_stream << "0x";
  for (uint i = 0; i < bv.size(); i++) {
      word.set(32 - 1 - (i % 32), bv[i]);
      if (i % 32 == 32 - 1 || i == bv.size() - 1) {
        str_stream << std::setfill('0') << std::setw(8) << std::hex << word.to_ulong();
      }
  }
  return str_stream.str();
}

#endif // SERIALIZATION_H