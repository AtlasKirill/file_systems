//
// Created by kirill on 03.10.18.
//

#ifndef CONVERTER_UTF_8_UTF_8_H
#define CONVERTER_UTF_8_UTF_8_H


#include <iostream>
#include <vector>
#include <exception>
#include <bitset>
#include <cmath>

using std::vector;
using std::cout;
using std::cin;
using std::endl;

int8_t number_of_octets(const uint32_t &number);
vector<uint8_t> to_utf8(const vector<uint32_t> &x);
vector<uint32_t> from_utf8(const vector<uint8_t> &x);


#endif //CONVERTER_UTF_8_UTF_8_H
