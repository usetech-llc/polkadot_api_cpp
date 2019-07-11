
#include "scale.h"
#include <cstring>
#include <fstream>
#include <iostream>
#include <sstream>

using namespace std;

namespace scale {
class CompactIntegerException : public exception {
    string _msg;

public:
    CompactIntegerException(string msg) { _msg = msg; };
    virtual char const *what() const noexcept { return _msg.c_str(); };
};

uint8_t nextWord(string &stream) {

    auto minor = stream.substr(0, 2);
    stream = stream.substr(2);
    auto major = stream.substr(0, 2);
    stream = stream.substr(2);
    return stoul(major + minor, nullptr, 16);
};

uint8_t nextByte(string &stream) {

    auto nb = stream.substr(0, 2);
    stream = stream.substr(2);
    return stoul(nb, nullptr, 16);
};

string extractString(string &stream, int len) {

    string s;
    while (len > 0) {
        s += nextByte(stream);
        len--;
    }
    return s;
};

CompactInteger decodeCompactInteger(string &stream) {
    auto first_byte = nextByte(stream);

    const uint8_t flag = (first_byte)&0b00000011u;

    size_t number = 0u;

    switch (flag) {
    case 0b00u: {
        number = static_cast<size_t>(first_byte >> 2u);
        break;
    }

    case 0b01u: {
        auto second_byte = nextByte(stream);

        number = (static_cast<size_t>((first_byte)&0b11111100u) + static_cast<size_t>(second_byte) * 256u) >> 2u;
        break;
    }

    case 0b10u: {
        number = first_byte;
        size_t multiplier = 256u;

        for (auto i = 0u; i < 3u; ++i) {
            number += (nextByte(stream)) * multiplier;
            multiplier = multiplier << 8u;
        }
        number = number >> 2u;
        break;
    }

    case 0b11: {
        auto bytes_count = ((first_byte) >> 2u) + 4u;
        CompactInteger multiplier{1u};
        CompactInteger value = 0;

        // we assured that there are m more bytes,
        // no need to make checks in a loop
        for (auto i = 0u; i < bytes_count; ++i) {
            value += (nextByte(stream)) * multiplier;
            multiplier *= 256u;
        }

        return value;
    }

    default:
        throw CompactIntegerException("CompactInteger decode error: unknown flag");
    }

    return CompactInteger{(long)number};
};

CompactIntegerLEBytes encodeCompactInteger(unsigned __int128 n) {

    CompactIntegerLEBytes b;
    memset(&b, 0, sizeof(b));

    if (n <= 63) {
        b.length = 1;
        b.bytes[0] = n << 2;
    } else if (n <= 0x3FFF) {
        b.length = 2;
        b.bytes[0] = ((n & 0x3F) << 2) | 0x01;
        b.bytes[1] = ((n & 0xFC0) >> 6);
    } else if (n <= 0x3FFFFFFF) {
        b.length = 4;
        b.bytes[0] = ((n & 0x3F) << 2) | 0x02;
        n >>= 6;
        for (int i = 1; i < 4; ++i) {
            b.bytes[i] = (n & 0xFF);
            n >>= 8;
        }
    } else { // Big integer mode
        b.length = 1;
        int byteNum = 1;
        while (n) {
            b.bytes[byteNum++] = (n & 0xFF);
            n >>= 8;
        }
        b.length = byteNum;
        b.bytes[0] = ((byteNum - 5) << 2) | 0x03;
    }

    return move(b);
}

long writeCompactToBuf(CompactIntegerLEBytes ci, uint8_t *buf) {
    for (int i = 0; i < ci.length; ++i)
        buf[i] = ci.bytes[i];
    return ci.length;
}

} // namespace scale
