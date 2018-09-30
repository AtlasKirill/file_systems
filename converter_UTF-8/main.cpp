#include <iostream>
#include <vector>
#include <exception>
#include <bitset>
#include <cmath>

using std::vector;
using std::cout;
using std::cin;


int8_t number_of_octets(const uint32_t &a) {
    if (a < static_cast<uint32_t >(1 << 7))
        return 1;
    if (a < static_cast<uint32_t >(1 << 11))
        return 2;
    if (a < static_cast<uint32_t >(1 << 16))
        return 3;
    if (a < static_cast<uint32_t >(1 << 21))
        return 4;
    throw std::invalid_argument("number overflows symbol's number in UTF");
}

vector<uint8_t> to_utf8(vector<uint32_t> &x);

vector<uint8_t> to_utf8(vector<uint32_t> &x) {
    int8_t bytes_num = 0;

    vector<uint8_t> result;
    for (auto number: x) {
        bytes_num = number_of_octets(number);
        if (bytes_num == 1)
            result.push_back((uint8_t) number);
        if (bytes_num == 2) {
            result.push_back(0b11000000 | (number >> 6));
            result.push_back(0b10000000 | (0b00111111 & (uint8_t) number));
        }
        if (bytes_num == 3) {
            result.push_back(0b11100000 | (number >> 12));
            result.push_back(0b10000000 | (0b00111111 & (uint8_t) (number>>6)));
            result.push_back(0b10000000 |(0b00111111 & (uint8_t) number));
        }
        if (bytes_num ==4){
//            result.push_back(0b11100000 | (number >> 18));
//            result.push_back()
        }
    }

    return result;
}

vector<uint32_t> from_utf8(vector<uint8_t> &x);

int main() {

    return 0;
}



