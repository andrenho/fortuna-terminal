#include "threadrunner.hh"

#include <csignal>

#include <algorithm>
#include <thread>

void ThreadRunner::run_comm_threads(bool debug_comm)
{
    if (comm_.channels() == Channels::InputAndOutput) {
        auto comm_io = dynamic_cast<CommIO*>(&comm_);
        input_thread_ = std::make_unique<std::thread>(&ThreadRunner::input_thread, this, comm_io, debug_comm);
        output_thread_ = std::make_unique<std::thread>(&ThreadRunner::output_thread, this, comm_io, debug_comm);
    }
}

void ThreadRunner::finalize_comm_threads()
{
    threads_running_ = false;

    input_queue_.push({});  // release the locks
    output_queue_.push({});

    if (comm_.release_locks()) {
        input_thread_->join();
    } else {
        pthread_kill(input_thread_->native_handle(), 9);
        input_thread_->detach();
    }

    output_thread_->join();
}

void ThreadRunner::input_thread(CommIO *comm_io, bool debug_comm) const
{
    while (threads_running_) {
        auto byte = comm_io->read_blocking();
        if (byte.has_value()) {
            input_queue_.push(byte.value());
            if (debug_comm)
                debug_byte(byte.value(), true);
        }
    }
}

void ThreadRunner::output_thread(CommIO *comm_io, bool debug_comm) const
{
    while (threads_running_) {
        std::vector<uint8_t> bytes_to_output;
        output_queue_.pop_all_into(bytes_to_output);
        comm_io->write(bytes_to_output);
        if (debug_comm)
            std::for_each(bytes_to_output.begin(), bytes_to_output.end(), [this](uint8_t byte) { debug_byte(byte, false); });
    }
}

void ThreadRunner::debug_byte(uint8_t byte, bool is_input) const
{
    std::unique_lock<std::mutex> lock(*mutex_);
#if COLOR_TERMINAL
    printf("\e[1;%dm", is_input ? 33 : 34);
#else
    putchar(is_input ? '>' : '<');
#endif
    if (byte >= 32 && byte < 127)
        printf("%c", byte);
    else if (byte == '\e')
        printf("\n[ESC]");
    else
        printf("[%02X]", byte);
    if (byte == 10)
        printf("\n");
#if COLOR_TERMINAL
    printf("\e[0m");
#endif
    fflush(stdout);
}
