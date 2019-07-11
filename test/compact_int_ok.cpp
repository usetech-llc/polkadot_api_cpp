#include "../src/polkadot.h"
#undef NDEBUG
#include <cassert>

int main(int argc, char *argv[]) {

    // 141 - 2-byte mode
    auto ci = scale::encodeCompactInteger(141);
    // printf("%02X %02X\n", ci.bytes[0], ci.bytes[1]);
    assert(ci.length == 2);
    assert(ci.bytes[0] == 0x35);
    assert(ci.bytes[1] == 0x02);

    // 1 - 1 byte mode
    ci = scale::encodeCompactInteger(1);
    assert(ci.length == 1);
    assert(ci.bytes[0] == 0x04);

    // 1 mDOT - bit integer mode
    ci = scale::encodeCompactInteger(1000000000000);
    // printf("%d\n", ci.length);
    // for (int i = 0; i < ci.length; ++i)
    //    printf("%02X ", ci.bytes[i]);
    // printf("\n");

    assert(ci.length == 6);
    assert(ci.bytes[0] == 0x07);
    assert(ci.bytes[1] == 0x00);
    assert(ci.bytes[2] == 0x10);
    assert(ci.bytes[3] == 0xA5);
    assert(ci.bytes[4] == 0xD4);
    assert(ci.bytes[5] == 0xE8);

    cout << "success" << endl;

    return 0;
}
