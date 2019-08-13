#include <chrono>
#include <condition_variable>
#include <iostream>
#include <map>
#include <mutex>
#include <queue>
#include <string>
#include <thread>

using namespace std;

class Semaphore {
public:
    Semaphore(int count_ = 0) : count(count_) {}

    inline void notify() {
        std::unique_lock<std::mutex> lock(mtx);
        count++;
        cv.notify_one();
    }
    inline void wait() {
        std::unique_lock<std::mutex> lock(mtx);
        while (count == 0) {
            cv.wait(lock);
        }
        count--;
    }

private:
    std::mutex mtx;
    std::condition_variable cv;
    int count;
};

template <typename T, typename K> class ConcurrentMapQueue {
private:
    map<K, queue<T>> _collection;
    map<K, chrono::time_point<chrono::steady_clock>> _timing;
    bool canRead(K key);
    Semaphore *_semR;
    Semaphore *_semW;
    void write(K &key, T &value);
    T read(K &key);

public:
    ConcurrentMapQueue(int max_size);
    ~ConcurrentMapQueue();
    void put(K &key, T &value);
    T get(K &key);
    vector<K> getKeys();
    unsigned long getKeyAge(K &);
};

template <typename T, typename K> ConcurrentMapQueue<T, K>::ConcurrentMapQueue(int max_size) {
    _semR = new Semaphore(1);
    _semW = new Semaphore(max_size);
}

template <typename T, typename K> ConcurrentMapQueue<T, K>::~ConcurrentMapQueue() {
    delete _semR;
    delete _semW;
}

template <typename T, typename K> bool ConcurrentMapQueue<T, K>::canRead(K key) { return _collection[key].size() > 0; }

template <typename T, typename K> void ConcurrentMapQueue<T, K>::write(K &key, T &value) {
    if (_collection.find(key) == _collection.end()) {
        auto first = std::pair<const K, queue<T>>(key, queue<T>());
        first.second.push(value);
        _collection.insert(first);
        _timing.insert(make_pair(key, chrono::steady_clock::now()));
    } else {
        _collection[key].push(value);
        _timing[key] = chrono::steady_clock::now();
    }
}

template <typename T, typename K> T ConcurrentMapQueue<T, K>::read(K &key) {
    auto value = &_collection[key];
    auto el = value->front();
    value->pop();
    return el;
}

template <typename T, typename K> void ConcurrentMapQueue<T, K>::put(K &key, T &value) {
    _semW->wait();
    write(key, value);
    _semW->notify();
}

template <typename T, typename K> T ConcurrentMapQueue<T, K>::get(K &key) {
    _semR->wait();
    while (!canRead(key)) {
        _semR->wait();
    }

    auto result = read(key);
    _semR->notify();
    return result;
}

template <typename T, typename K> vector<K> ConcurrentMapQueue<T, K>::getKeys() {

    vector<K> keys;
    for (auto const &element : _collection) {
        keys.push_back(element.first);
    }
    return keys;
}

template <typename T, typename K> unsigned long ConcurrentMapQueue<T, K>::getKeyAge(K &key) {
    if (_timing.count(key) > 0) {
        return chrono::duration_cast<std::chrono::milliseconds>(chrono::steady_clock::now() - _timing[key]).count();
    }
    return 0;
}
