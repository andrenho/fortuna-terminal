#ifndef SYNCQUEUE_HH_
#define SYNCQUEUE_HH_

// C++ implementation of the above approach
#include <condition_variable>
#include <iostream>
#include <mutex>
#include <queue>

// Thread-safe queue_
template <typename T>
class SyncQueue {
private:
    // Underlying queue_
    std::queue<T> m_queue;

    // mutex for thread synchronization
    std::mutex m_mutex;

    // Condition variable for signaling
    std::condition_variable m_cond;

public:
    // Pushes an element to the queue_
    void push(T item)
    {

        // Acquire lock
        std::unique_lock<std::mutex> lock(m_mutex);

        // Add item
        m_queue.push(item);

        // Notify one thread that
        // is waiting
        m_cond.notify_one();
    }

    // Pops an element off the queue_
    T pop()
    {

        // acquire lock
        std::unique_lock<std::mutex> lock(m_mutex);

        // wait until queue_ is not empty
        m_cond.wait(lock,
                    [this]() { return !m_queue.empty(); });

        // retrieve item
        T item = m_queue.front();
        m_queue.pop();

        // return item
        return item;
    }
};

#endif //SYNCQUEUE_HH_
