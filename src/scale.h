
#include <fstream>
#include <iostream>
#include <sstream>

#define CompactInteger long
//--------------------------- SCALE Decoder
using namespace std;
namespace scale {

uint8_t nextWord(std::string &stream);
uint8_t nextByte(string &stream);
string extractString(string &stream, int len);
CompactInteger decodeCompactInteger(string &stream);
}