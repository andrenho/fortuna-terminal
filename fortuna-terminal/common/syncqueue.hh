#ifndef SYNCQUEUE_HH_
#define SYNCQUEUE_HH_

#include <condition_variable>
#include <mutex>
#include <optional>
#include <queue>
#include "noncopyable.hh"

template <typename T>
class SyncQueue : NonCopyable {
public:
    void emplace(T&& item) {
        std::unique_lock<std::mutex> lock(mutex_);
        queue_.emplace(std::move(item));
        cond_.notify_one();
    }

    T pop_block() {
        std::unique_lock<std::mutex> lock(mutex_);
        cond_.wait(lock, [this]() { return !queue_.empty(); });

        T item = std::move(queue_.front());
        queue_.pop();

        return item;
    }

    std::optional<T> pop_nonblock() {
        std::unique_lock<std::mutex> lock(mutex_);
        if (queue_.empty()) {
            return {};
        } else {
            T item = std::move(queue_.front());
            queue_.pop();
            return item;
        }
    }

    template <typename F>
    void for_each(F f) {
        while (!empty()) {
            T obj = pop_block();
            f(std::move(obj));
        }
    }

    bool empty() {
        std::unique_lock<std::mutex> lock(mutex_);
        return queue_.empty();
    }

    size_t size() {
        std::unique_lock<std::mutex> lock(mutex_);
        return queue_.size();
    }

private:
    std::queue<T>           queue_;
    std::mutex              mutex_;
    std::condition_variable cond_;
};

#endif //SYNCQUEUE_HH_
