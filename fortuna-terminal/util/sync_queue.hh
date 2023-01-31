#ifndef SYNCQUEUE_HH_
#define SYNCQUEUE_HH_

// C++ implementation of the above approach
#include <condition_variable>
#include <iostream>
#include <mutex>
#include <queue>
#include <optional>

template <typename T>
class SyncQueue {
private:
    std::queue<T> m_queue;
    std::mutex m_mutex;
    std::condition_variable m_cond;

public:
    void enqueue(T item)
    {
        std::unique_lock<std::mutex> lock(m_mutex);
        m_queue.push(item);
        m_cond.notify_one();
    }

    T dequeue_block()
    {
        std::unique_lock<std::mutex> lock(m_mutex);

        // wait until queue is not empty
        m_cond.wait(lock,
                    [this]() { return !m_queue.empty(); });

        T item = m_queue.front();
        m_queue.pop();

        return item;
    }

    std::optional<T> dequeue_noblock()
    {
        std::unique_lock<std::mutex> lock(m_mutex);

        if (m_queue.empty())
            return {};

        T item = m_queue.front();
        m_queue.pop();

        return item;
    }

    void release_lock()
    {
        enqueue(T());
    }
};

#endif //SYNCQUEUE_HH_
