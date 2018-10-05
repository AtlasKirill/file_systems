#include "utf_8.h"


int8_t number_of_octets(const uint32_t &number) {
    if (number < static_cast<uint32_t >(1 << 7))
        return 1;
    if (number < static_cast<uint32_t >(1 << 11))
        return 2;
    if (number < static_cast<uint32_t >(1 << 16))
        return 3;
    if (number < static_cast<uint32_t >(1 << 21))
        return 4;
    throw std::runtime_error("Number overflows symbol's number in UTF");
}

static inline uint8_t number_of_symbol_bytes(const uint8_t &byte) {
    if (byte >> 7 == 0)
        return 1;
    if (byte >> 5 == 0b110)
        return 2;
    if (byte >> 4 == 0b1110)
        return 3;
    if (byte >> 3 == 0b11110)
        return 4;
    throw std::runtime_error("Bad imput (wrong format of the byte");
}

vector<uint8_t> to_utf8(vector<uint32_t> &x) {
    auto shift_shrink = [](const uint32_t &number, uint8_t shift) {
        return 0b10000000 | (0b00111111 & static_cast<uint8_t >(number >> shift));
    };

    int8_t bytes_num = 0;

    vector<uint8_t> result;
    for (auto number: x) {
        bytes_num = number_of_octets(number);
        if (bytes_num == 1) {
            result.push_back((uint8_t) number);
            continue;
        }
        if (bytes_num == 2) {
            result.push_back(0b11000000 | (number >> 6));
            result.push_back(shift_shrink(number,0));
            continue;
        }
        if (bytes_num == 3) {
            result.push_back(0b11100000 | (number >> 12));
            result.push_back(shift_shrink(number,6));
            result.push_back(shift_shrink(number,0));
            continue;
        }
        if (bytes_num == 4) {
            result.push_back(0b11110000 | (number >> 18));
            result.push_back(shift_shrink(number,12));
            result.push_back(shift_shrink(number,6));
            result.push_back(shift_shrink(number,0));
            continue;
        }
    }
    return result;
}


vector<uint32_t> from_utf8(vector<uint8_t> &x) {

    //extracting information from 2d,3th,4th bytes
    auto shrink_shift = [](const uint8_t &x, uint8_t shift) {
        return static_cast<uint32_t >(0b00111111 & x) << shift;
    };

    int8_t bytes_num = 0;
    vector<uint32_t> result;
    for (auto number = x.begin(); number != x.end();) {
        bytes_num = number_of_symbol_bytes(*number);
        if (bytes_num == 1) {
            result.push_back(*number);
            ++number;
            continue;
        }
        if (bytes_num == 2) {
            result.push_back((((uint32_t) 0b00011111 & (*number)) << 6) + shrink_shift(*++number, 0));
            ++number;
            continue;
        }
        if (bytes_num == 3) {
            result.push_back((((uint32_t) 0b00001111 & (*number)) << 12) +
                             shrink_shift(*++number, 6) +
                             shrink_shift(*++number, 0));
            ++number;
            continue;
        }
        if (bytes_num == 4) {
            result.push_back((((uint32_t) 0b00000111 & (*number)) << 18) +
                             shrink_shift(*++number, 12) +
                             shrink_shift(*++number, 6) +
                             shrink_shift(*++number, 0));
            ++number;
            continue;
        }
    }
    return result;
}

//int main() {
//    uint32_t a = 66376;
//    vector<uint32_t> b = {a};
//    vector<uint8_t> n = to_utf8(b);
//    vector<uint8_t> a = {240, 144, 141, 136};
//    vector<uint32_t> n = from_utf8(a);
//    return 0;
//}



