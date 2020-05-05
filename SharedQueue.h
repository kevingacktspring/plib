//
// Created by gackt on 5/5/20.
//

#ifndef PLIB_SHAREDQUEUE_H
#define PLIB_SHAREDQUEUE_H

#include <queue>
#include <mutex>
#include <condition_variable>
#include <pthread.h>

template<typename T>
class SharedQueue {
public:
    SharedQueue();

    SharedQueue(T &&item);

    ~SharedQueue();

    T &front();

    bool front(T &);

    void pop_front();

    void push_back(const T &item);

    void push_back(T &&item);

    size_t size();

    bool empty();

private:
    std::deque<T> queue_;
    std::mutex mutex_;
    std::condition_variable cond_;

    pthread_mutex_t lock = PTHREAD_MUTEX_INITIALIZER;
};

template<typename T>
SharedQueue<T>::SharedQueue(T &&item) {
    SharedQueue();
    this->push_back(item);
}

template<typename T>
SharedQueue<T>::SharedQueue() {
    if (pthread_mutex_init(&lock, NULL) != 0) {
        printf("\n mutex intit failed \n");
        exit(-1);
    }
}

template<typename T>
SharedQueue<T>::~SharedQueue() {
    pthread_mutex_destroy(&lock);
}

template<typename T>
bool SharedQueue<T>::front(T &t) {
    if (queue_.empty()) {
        return false;
    }
    t = std::move(queue_.front());
    return true;
}

template<typename T>
void SharedQueue<T>::pop_front() {
    if (!queue_.empty()) {
        queue_.pop_front();
    }
}

template<typename T>
void SharedQueue<T>::push_back(const T &item) {
    queue_.push_back(item);
}

template<typename T>
void SharedQueue<T>::push_back(T &&item) {
    queue_.push_back(std::move(item));
}

template<typename T>
size_t SharedQueue<T>::size() {
    std::unique_lock<std::mutex> mlock(mutex_);
    size_t size = queue_.size();
    mlock.unlock();
    return size;
}

template<typename T>
bool SharedQueue<T>::empty() {
    return queue_.empty();
}

#endif //PLIB_SHAREDQUEUE_H
