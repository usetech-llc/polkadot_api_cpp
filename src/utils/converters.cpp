#include "../../src/polkadot.h"

uint8_t fromHexByte(const char *byteStr) {
    unsigned char digit1 = byteStr[0];
    unsigned char digit2 = byteStr[1];
    unsigned char byte = 0;
    if ((digit1 >= 'a') && (digit1 <= 'f'))
        digit1 = digit1 - 'a' + 10;
    else if ((digit1 >= 'A') && (digit1 <= 'F'))
        digit1 = digit1 - 'A' + 10;
    else if ((digit1 >= '0') && (digit1 <= '9'))
        digit1 = digit1 - '0';
    if ((digit2 >= 'a') && (digit2 <= 'f'))
        digit2 = digit2 - 'a' + 10;
    else if ((digit2 >= 'A') && (digit2 <= 'F'))
        digit2 = digit2 - 'A' + 10;
    else if ((digit2 >= '0') && (digit2 <= '9'))
        digit2 = digit2 - '0';

    byte = (digit1 << 4) | digit2;
    return byte;
}

uint128 atoi128(const char *s) {
    while (*s == ' ' || *s == '\t' || *s == '\n' || *s == '+')
        ++s;
    int sign = 1;
    if (*s == '-') {
        ++s;
        sign = -1;
    }
    size_t digits = 0;
    while (s[digits] >= '0' && s[digits] <= '9')
        ++digits;
    char scratch[digits];
    for (size_t i = 0; i < digits; ++i)
        scratch[i] = s[i] - '0';
    size_t scanstart = 0;

    uint128 result = 0;
    uint128 mask = 1;
    while (scanstart < digits) {
        if (scratch[digits - 1] & 1)
            result |= mask;
        mask <<= 1;
        for (size_t i = digits - 1; i > scanstart; --i) {
            scratch[i] >>= 1;
            if (scratch[i - 1] & 1)
                scratch[i] |= 8;
        }
        scratch[scanstart] >>= 1;
        while (scanstart < digits && !scratch[scanstart])
            ++scanstart;
        for (size_t i = scanstart; i < digits; ++i) {
            if (scratch[i] > 7)
                scratch[i] -= 3;
        }
    }

    return result * sign;
}

template <typename T> T fromHex(string hexStr, bool bigEndianBytes) {
    int offset = 0;
    int byteOffset = 0;
    if ((hexStr[0] == '0') && (hexStr[1] == 'x')) {
        offset = 2;
    }
    T result = 0;
    while (offset < (int)hexStr.length()) {
        unsigned char byte = fromHexByte(hexStr.c_str() + offset);
        if (bigEndianBytes) {
            result = (result << 8) | byte;
        } else {
            T wbyte = byte;
            result = (wbyte << byteOffset) | result;
            byteOffset += 8;
        }

        offset += 2;
    }
    return result;
}

template <> vector<uint8_t> fromHex<vector<uint8_t>>(string hexStr, bool bigEndianBytes) {
    int offset = 0;
    int start = 0;
    if ((hexStr[0] == '0') && (hexStr[1] == 'x')) {
        start = 2;
    }
    if (bigEndianBytes)
        offset = start;
    else
        offset = hexStr.length() - 2;

    vector<uint8_t> result;
    while (true) {
        unsigned char byte = fromHexByte(hexStr.c_str() + offset);

        result.push_back(byte);
        if (bigEndianBytes) {
            offset += 2;
            if (offset >= (int)hexStr.length())
                break;
        } else {
            offset -= 2;
            if (offset < start)
                break;
        }
    }
    return result;
}

template long long fromHex<long long>(string hexStr, bool bigEndianBytes);
template unsigned long fromHex<unsigned long>(string hexStr, bool bigEndianBytes);
template uint128 fromHex<uint128>(string hexStr, bool bigEndianBytes);
