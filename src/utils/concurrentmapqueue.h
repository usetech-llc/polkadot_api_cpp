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
    Semaphore(int count = 0) : _count(count) {}

    inline void notify() {
        std::unique_lock<std::mutex> lock(_mtx);
        _count++;
        _cv.notify_one();
    }
    inline void wait() {
        std::unique_lock<std::mutex> lock(_mtx);
        while (_count == 0) {
            _cv.wait(lock);
        }
        _count--;
    }
    void unblockAll() {
        std::unique_lock<std::mutex> lock(_mtx);
        _count = 1;
        _cv.notify_all();
    }

private:
    std::mutex _mtx;
    std::condition_variable _cv;
    int _count;
    int _max_count;
};

template <typename T, typename K> class ConcurrentMapQueue {
private:
    map<K, queue<T>> _collection;
    map<K, Semaphore *> _semR;
    map<K, Semaphore *> _semW;
    int _max_size;
    void write(K &key, T &value);
    bool read(K &key, T *buf);
    void addKey(K &key);
    std::mutex _keyMtx;

public:
    ConcurrentMapQueue(int max_size);
    ~ConcurrentMapQueue();
    void put(K &key, T &value);
    bool get(K &key, T *buf);
    vector<K> getKeys();

    void eraseKey(K &key);
};

template <typename T, typename K> ConcurrentMapQueue<T, K>::ConcurrentMapQueue(int max_size) { _max_size = max_size; }
template <typename T, typename K> ConcurrentMapQueue<T, K>::~ConcurrentMapQueue() {}

template <typename T, typename K> void ConcurrentMapQueue<T, K>::write(K &key, T &value) {
    _collection[key].push(value);
}

template <typename T, typename K> bool ConcurrentMapQueue<T, K>::read(K &key, T *buf) {
    if (_collection[key].size() > 0) {
        auto value = &_collection[key];
        *buf = value->front();
        value->pop();
        return true;
    }
    return false;
}

template <typename T, typename K> void ConcurrentMapQueue<T, K>::put(K &key, T &value) {
    addKey(key);

    _semW[key]->wait();
    write(key, value);
    _semR[key]->notify();
}

template <typename T, typename K> bool ConcurrentMapQueue<T, K>::get(K &key, T *buf) {
    addKey(key);

    _semR[key]->wait();
    bool result = read(key, buf);
    if (result) // If false, the key is being erased, semaphore will be deleted
        _semW[key]->notify();
    return result;
}

template <typename T, typename K> vector<K> ConcurrentMapQueue<T, K>::getKeys() {
    vector<K> keys;
    for (auto const &element : _collection) {
        keys.push_back(element.first);
    }
    return keys;
}

template <typename T, typename K> void ConcurrentMapQueue<T, K>::addKey(K &key) {
    std::unique_lock<std::mutex> lock(_keyMtx);
    if (_semR.count(key) == 0) {
        _semR[key] = new Semaphore(0);
        _semW[key] = new Semaphore(_max_size - 1);
    }
}

template <typename T, typename K> void ConcurrentMapQueue<T, K>::eraseKey(K &key) {
    Semaphore *semR = _semR[key];
    Semaphore *semW = _semW[key];

    // Unblock any blocked consumers, they will get no response and false result
    semR->unblockAll();

    _semR.erase(key);
    _semW.erase(key);

    delete semR;
    delete semW;
}
