#ifndef SYNCQUEUE_HH_
#define SYNCQUEUE_HH_

#include <condition_variable>
#include <mutex>
#include <optional>
#include <queue>
#include "common/types/noncopyable.hh"

template <typename T>
class SyncQueue : NonCopyable {
public:
    SyncQueue() : max_sz_(std::numeric_limits<size_t>::max()) {}
    explicit SyncQueue(size_t max_sz) : max_sz_(max_sz) {}

    void emplace(T&& item) {
        std::unique_lock<std::mutex> lock(mutex_);
        queue_.emplace(std::move(item));
        cond_.notify_one();
    }

    template <typename ...Args>
    size_t emplace(Args const&... args) {
        std::unique_lock<std::mutex> lock(mutex_);
        if (queue_.size() >= max_sz_ - 1)
            return 0;
        queue_.emplace(args...);
        cond_.notify_one();
        return 1;
    }

    size_t push(T item) {
        std::unique_lock<std::mutex> lock(mutex_);
        if (queue_.size() >= max_sz_ - 1)
            return 0;
        queue_.push(item);
        cond_.notify_one();
        return 1;
    }

    template <typename U>
    size_t push_all(U const& collection) {
        std::unique_lock<std::mutex> lock(mutex_);

        size_t collection_sz = std::size(collection);
        size_t items_to_push = collection_sz;
        if (queue_.size() + items_to_push >= (max_sz_ - 1))
            items_to_push = max_sz_ - queue_.size();

        for (size_t i = 0; i < items_to_push; ++i)
            queue_.push(collection[i]);

        cond_.notify_one();

        return items_to_push;
    }

    T pop_block() {
        std::unique_lock<std::mutex> lock(mutex_);
        cond_.wait(lock, [this]() { return !queue_.empty(); });

        T item = std::move(queue_.front());
        queue_.pop();

        return item;
    }

    template <typename U>
    void pop_all_into(U& collection) {
        std::unique_lock<std::mutex> lock(mutex_);
        cond_.wait(lock, [this]() { return !queue_.empty(); });

        while (!queue_.empty()) {
            collection.push_back(queue_.front());
            queue_.pop();
        }
    }

    template <typename U>
    void pop_all_into_noblock(U& collection) {
        std::unique_lock<std::mutex> lock(mutex_);

        while (!queue_.empty()) {
            collection.push_back(queue_.front());
            queue_.pop();
        }
    }

    template <typename U>
    void optionally_pop_all_into(U& collection) {
        std::unique_lock<std::mutex> lock(mutex_);
        if (queue_.empty())
            return;

        while (!queue_.empty()) {
            collection.push_back(queue_.front());
            queue_.pop();
        }
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

    bool empty() {
        std::unique_lock<std::mutex> lock(mutex_);
        return queue_.empty();
    }

    size_t size() {
        std::unique_lock<std::mutex> lock(mutex_);
        return queue_.size();
    }

    void clear() {
        std::unique_lock<std::mutex> lock(mutex_);
        while (!queue_.empty())
            queue_.pop();
    }

private:
    size_t                  max_sz_ {};
    std::queue<T>           queue_ {};
    std::mutex              mutex_ {};
    std::condition_variable cond_ {};
};

template <class T>
using SyncQueueUniqPtr = std::unique_ptr<SyncQueue<T>>;

using SyncQueueByte = SyncQueue<uint8_t>;
extern template class SyncQueue<uint8_t>;

#endif //SYNCQUEUE_HH_
