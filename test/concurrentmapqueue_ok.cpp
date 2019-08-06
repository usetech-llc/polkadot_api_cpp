#include "../src/utils/concurrentmapqueue.h"
#include <cassert>

using namespace std;

int main(int argc, char *argv[]) {
    ConcurrentMapQueue<int, string> mq(10);

    string k1("test1");

    int i1 = 1, i2 = 2, i3 = 3;

    mq.put(k1, i1);
    mq.put(k1, i2);
    mq.put(k1, i3);

    assert(mq.get(k1) == i1);
    assert(mq.get(k1) == i2);
    assert(mq.get(k1) == i3);

    cout << "success" << endl;

    return 0;
}
