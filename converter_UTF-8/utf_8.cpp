#include "utf_8.h"

enum States {
    first_bit, two_bits, three_bits, four_bits, other_bits
};


bool consistency_check(const vector<uint8_t> &x) {
    States state = first_bit;
    uint8_t bit_counter = 0;
    for (const auto &item:x) {
        const uint8_t cur_bit = item;
        switch (state) {
            case first_bit:
                if (cur_bit >> 7 == 0)
                    continue;
                else if (cur_bit >> 5 == 0b110) {
                    bit_counter++;
                    state = two_bits;
                    continue;
                } else if (cur_bit >> 4 == 0b1110) {
                    bit_counter++;
                    state = three_bits;
                    continue;
                } else if (cur_bit >> 3 == 0b11110) {
                    bit_counter++;
                    state = four_bits;
                    continue;
                }
                return false;
            case two_bits:
                if (cur_bit >> 6 == 0b10) {
                    state = first_bit;
                    bit_counter = 0;
                    continue;
                } else
                    return false;
            case three_bits:
                if (cur_bit >> 6 == 0b10 && bit_counter < 2) {
                    bit_counter++;
                    continue;
                } else if (cur_bit >> 6 == 0b10 && bit_counter == 2) {
                    state = first_bit;
                    bit_counter = 0;
                    continue;
                } else
                    return false;
            case four_bits:
                if (cur_bit >> 6 == 0b10 && bit_counter < 3) {
                    bit_counter++;
                    continue;
                } else if (cur_bit >> 6 == 0b10 && bit_counter == 3) {
                    state = first_bit;
                    bit_counter = 0;
                    continue;
                } else
                    return false;
        }
    }
    return true;

}

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

uint8_t number_of_symbol_bytes(const uint8_t &byte) {
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

vector<uint8_t> to_utf8(const vector<uint32_t> &x) {
    auto shift_shrink = [](const uint32_t &number, uint8_t shift) {
        return 0b10000000 | (0b00111111 & static_cast<uint8_t >(number >> shift));
    };

    vector<uint8_t> result;
    for (const auto number: x) {
        int8_t bytes_num = number_of_octets(number);
        if (bytes_num == 1) {
            result.push_back((uint8_t) number);
            continue;
        } else if (bytes_num == 2) {
            result.push_back(0b11000000 | (number >> 6));
            result.push_back(shift_shrink(number, 0));
            continue;
        } else if (bytes_num == 3) {
            result.push_back(0b11100000 | (number >> 12));
            result.push_back(shift_shrink(number, 6));
            result.push_back(shift_shrink(number, 0));
            continue;
        } else {
            result.push_back(0b11110000 | (number >> 18));
            result.push_back(shift_shrink(number, 12));
            result.push_back(shift_shrink(number, 6));
            result.push_back(shift_shrink(number, 0));
            continue;
        }
    }
    return result;
}


vector<uint32_t> from_utf8(const vector<uint8_t> &x) {

    if (!consistency_check(x))
        throw std::runtime_error("Wrong bytes in input stream");
    //extracting information from 2d,3th,4th bytes
    auto shrink_shift = [](const uint8_t &x, uint8_t shift) {
        return static_cast<uint32_t >(0b00111111 & x) << shift;
    };

    vector<uint32_t> result;
    for (auto number = x.cbegin(); number != x.cend();) {
        int8_t bytes_num = number_of_symbol_bytes(*number);
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



