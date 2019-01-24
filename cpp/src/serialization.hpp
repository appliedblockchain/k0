#ifndef SERIALIZATION_H
#define SERIALIZATION_H

#include <string>
#include <vector>

unsigned long hex2long(std::string hex_string);
std::vector<bool> hex2bits(std::string input);
std::vector<std::vector<bool>> hex2bitsVec(std::vector<std::string> input);
std::vector<std::vector<std::vector<bool>>> hex2bitsVecVec(std::vector<std::vector<std::string>> input);

std::string bits2hex(std::vector<bool> input);
#endif // SERIALIZATION_H