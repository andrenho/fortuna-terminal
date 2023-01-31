#ifndef SYNCQUEUE_HH_
#define SYNCQUEUE_HH_

// C++ implementation of the above approach
#include <condition_variable>
#include <iostream>
#include <mutex>
#include <queue>
#include <optional>

// Thread-safe queue_
template <typename T>
class SyncQueue {
private:
    // Underlying queue_
    std::queue<T> m_queue;

    // mutex for thread synchronization
    std::mutex m_mutex;

public:
    // Pushes an element to the queue_
    void push(T item)
    {
        std::unique_lock<std::mutex> lock(m_mutex);
        m_queue.push(item);
    }

    // Pops an element off the queue_
    std::optional<T> pop()
    {
        std::unique_lock<std::mutex> lock(m_mutex);

        if (m_queue.empty())
            return {};

        T item = m_queue.front();
        m_queue.pop();

        return item;
    }
};

#endif //SYNCQUEUE_HH_
