#ifndef ITERATIVETHREAD_HH_
#define ITERATIVETHREAD_HH_

#include <csignal>

#include <functional>
#include <thread>
#include <condition_variable>
#include <utility>

#include "common/types/noncopyable.hh"
#include "types/time.hh"

class IterativeThread : NonCopyable {
public:
    template<typename Callable, typename... Args>
    void run_without_wait(Callable&& f, Args&&... args) {
        thread_ = std::make_unique<std::thread>([this](Callable&& f_, Args&&... args_) {
            while (running_) {
                std::invoke(f_, args_...);
            }
        }, std::forward<Callable>(f), std::forward<Args>(args)...);
    }

    template<typename Callable, typename... Args>
    void run_with_wait(Callable&& f, Args&&... args) {
        thread_ = std::make_unique<std::thread>([this](Callable&& f_, Args&&... args_) {
            while (running_) {
                std::unique_lock<std::mutex> lock(*mutex_);
                cond_->wait(lock, [this]{ return ready_; });
                ready_ = false;
                std::invoke(f_, args_...);
            }
        }, std::forward<Callable>(f), std::forward<Args>(args)...);
    }

    void notify()
    {
        ready_ = true;
        cond_->notify_one();
    }

    template<typename T>
    void finalize(Duration wait_until_kill , T finalization_action) {
        running_ = false;
        notify();
        finalization_action();
        std::this_thread::sleep_for(wait_until_kill);
        if (thread_->joinable())
            pthread_kill(thread_->native_handle(), 9);
        thread_->detach();
    }

    void finalize(Duration wait_until_kill) { finalize(wait_until_kill, []{}); }
    void finalize() { finalize(0ms, []{}); }

private:
    bool                                     running_ = true;
    bool                                     ready_ = false;
    std::unique_ptr<std::condition_variable> cond_ = std::make_unique<std::condition_variable>();
    std::unique_ptr<std::mutex>              mutex_ = std::make_unique<std::mutex>();
    std::unique_ptr<std::thread>             thread_;
};


#endif //ITERATIVETHREAD_HH_
