#include "../src/utils/concurrentmapqueue.h"
#include <cassert>
#include <unistd.h>

using namespace std;

string k1("test1");
ConcurrentMapQueue<int, string> mq(10);

int producedValue = 0;
int consumedValue = -1;
int previousConsumedValue = -1;

void consumerThread() {
    usleep(500000);

    for (int i = 0; i < 21; ++i) {
        bool result = mq.get(k1, &consumedValue);
        cout << "consume " << consumedValue << endl;

        if (result) {
            assert(consumedValue == (previousConsumedValue + 1));
            previousConsumedValue = consumedValue;
        }
    }
}

void unblockThread() {
    usleep(600000);
    cout << "cleaning up" << endl;
    mq.eraseKey(k1);
}

int main(int argc, char *argv[]) {

    int i1 = 1, i2 = 2, i3 = 3;

    std::thread consumer(&consumerThread);
    std::thread unblocker(&unblockThread);

    // Produce
    for (int i = 0; i < 20; ++i) {
        cout << "produce " << producedValue << endl;
        mq.put(k1, producedValue);
        producedValue++;
    }

    consumer.join();
    unblocker.join();

    cout << "success" << endl;

    return 0;
}
