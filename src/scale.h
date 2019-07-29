#ifndef SCALE_H
#define SCALE_H

#include <fstream>
#include <iostream>
#include <sstream>

typedef struct {
    long length;
    uint8_t bytes[8];
} CompactIntegerLEBytes;

#define CompactInteger long
//--------------------------- SCALE Encoder/Decoder
using namespace std;
namespace scale {

uint8_t nextWord(std::string &stream);
uint8_t nextByte(string &stream);
string extractString(string &stream, int len);
CompactInteger decodeCompactInteger(string &stream);

CompactIntegerLEBytes encodeCompactInteger(uint128 n);
long writeCompactToBuf(CompactIntegerLEBytes ci, uint8_t *buf);
} // namespace scale

#endif // SCALE_H
