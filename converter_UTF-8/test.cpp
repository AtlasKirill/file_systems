//
// Created by kirill on 03.10.18.
//

#include "utf_8.h"
#include <algorithm>

int main() {

    vector<uint8_t> test_from_utf = {0b00100100, 0b11000010, 0b10100010, 0b11100010,
                                   0b10000010, 0b10101100, 0b11110000,
                                   0b10010000, 0b10001101, 0b10001000};

    vector<uint32_t> test_to_utf = {0x0024, 0x00A2, 0x20AC, 0x10348};
    vector<uint8_t> result_8;
    vector<uint32_t> result_32;

    result_8 = to_utf8(test_to_utf);
    result_32 = from_utf8(test_from_utf);

    cout << "The first original symbol is \u0024" << std::dec << " - it's code in UTF " << test_to_utf[0] << endl;
    cout << "The second original symbol is \u00A2" << std::dec << " - it's code in UTF " << test_to_utf[1] << endl;
    cout << "The thirth original symbol is \u20AC" << std::dec << " - it's code in UTF " << test_to_utf[2] << endl;
    cout << "The fourth original symbol is \u10348" << std::dec << " - it's code in UTF " << test_to_utf[3] << endl;
    cout << "comparing their bits representation..."<<endl;

    if(!std::equal(test_from_utf.begin(),test_from_utf.end(),result_8.begin(),result_8.end()))
        cout<<"function to_utf8 doesn't work"<<endl;
    else
        cout<<"function to_utf8 works correctly"<<endl;
    if(!std::equal(test_to_utf.begin(),test_to_utf.end(),result_32.begin(),result_32.end()))
        cout<<"function from_utf8 doesn't work"<<endl;
    else
        cout<<"function from_utf8 works correctly"<<endl;
    return 0;
}