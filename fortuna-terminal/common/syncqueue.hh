#ifndef SYNCQUEUE_HH_
#define SYNCQUEUE_HH_

#include <condition_variable>
#include <mutex>
#include <optional>
#include <deque>
#include "common/types/noncopyable.hh"

template <typename T>
class SyncQueue : NonCopyable {
public:
    SyncQueue() : max_sz_(std::numeric_limits<size_t>::max()) {}
    explicit SyncQueue(size_t max_sz) : max_sz_(max_sz) {}

    void emplace(T&& item) {
        std::unique_lock<std::mutex> lock(mutex_);
        deque_.emplace_back(std::move(item));
        cond_.notify_one();
    }

    template <typename ...Args>
    size_t emplace(Args const&... args) {
        std::unique_lock<std::mutex> lock(mutex_);
        if (deque_.size() >= max_sz_ - 1)
            return 0;
        deque_.emplace_back(args...);
        cond_.notify_one();
        return 1;
    }

    size_t push(T item) {
        std::unique_lock<std::mutex> lock(mutex_);
        if (deque_.size() >= max_sz_ - 1)
            return 0;
        deque_.push_back(item);
        cond_.notify_one();
        return 1;
    }

    template <typename U>
    size_t push_all(U const& collection) {
        std::unique_lock<std::mutex> lock(mutex_);

        size_t deque_sz = deque_.size();
        size_t collection_sz = std::size(collection);
        size_t items_to_push = collection_sz;
        if (deque_sz + items_to_push >= (max_sz_ - 1))
            items_to_push = max_sz_ - deque_sz;

        std::copy(std::begin(collection), std::begin(collection) + items_to_push, std::back_inserter(deque_));

        cond_.notify_one();

        return items_to_push;
    }

    T pop_block() {
        std::unique_lock<std::mutex> lock(mutex_);
        cond_.wait(lock, [this]() { return !deque_.empty(); });

        T item = std::move(deque_.front());
        deque_.pop_back();

        return item;
    }

    template <typename U>
    void pop_all_into(U& collection) {
        std::unique_lock<std::mutex> lock(mutex_);
        cond_.wait(lock, [this]() { return !deque_.empty(); });

        std::copy(std::begin(deque_), std::end(deque_), std::back_inserter(collection));
        deque_.clear();
    }

    template <typename U>
    void pop_all_into_noblock(U& collection) {
        std::unique_lock<std::mutex> lock(mutex_);

        std::copy(std::begin(deque_), std::end(deque_), std::back_inserter(collection));
        deque_.clear();
    }

    template <typename U>
    void optionally_pop_all_into(U& collection) {
        std::unique_lock<std::mutex> lock(mutex_);
        if (deque_.empty())
            return;

        std::copy(std::begin(deque_), std::end(deque_), std::back_inserter(collection));
        deque_.clear();
    }

    std::optional<T> pop_nonblock() {
        std::unique_lock<std::mutex> lock(mutex_);

        if (deque_.empty()) {
            return {};
        } else {
            T item = std::move(deque_.front());
            deque_.pop_back();
            return item;
        }
    }

    bool empty() {
        std::unique_lock<std::mutex> lock(mutex_);
        return deque_.empty();
    }

    size_t size() {
        std::unique_lock<std::mutex> lock(mutex_);
        return deque_.size();
    }

    void clear() {
        std::unique_lock<std::mutex> lock(mutex_);
        deque_.clear();
    }

private:
    size_t                  max_sz_ {};
    std::deque<T>           deque_ {};
    std::mutex              mutex_ {};
    std::condition_variable cond_ {};
};

template <class T>
using SyncQueueUniqPtr = std::unique_ptr<SyncQueue<T>>;

using SyncQueueByte = SyncQueue<uint8_t>;
extern template class SyncQueue<uint8_t>;

#endif //SYNCQUEUE_HH_
