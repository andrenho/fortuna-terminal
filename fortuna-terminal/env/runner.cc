#include "runner.hh"
#include "common/syncqueue.hh"
#include "comm/comm.hh"
#include "comm/io/comm_io.hh"

#include <memory>
#include <thread>

#include <signal.h>

void Runner::run_comm_threads(bool debug_comm)
{
    // TODO - debug communication

    if (comm_.channels() == Channels::InputAndOutput) {
        auto comm_io = dynamic_cast<CommIO*>(&comm_);

        input_thread_ = std::make_unique<std::thread>([this, comm_io]() {
            while (threads_active_) {
                auto byte = comm_io->read_blocking();
                if (byte.has_value()) {
                    input_queue_.push(byte.value());
                    // debug_byte(true, byte.value());
                }
            }
        });

        output_thread_ = std::make_unique<std::thread>([this, comm_io]() {
            while (threads_active_) {
                std::vector<uint8_t> bytes_to_output;
                output_queue_.pop_all_into(bytes_to_output);
                comm_io->write(bytes_to_output);
                /*
                if (debug_comm__)
                    std::for_each(bytes_to_output.begin(), bytes_to_output.end(), [this](uint8_t byte) { debug_byte(false, byte); });
                    */
            }
        });
    }
}

void Runner::finalize_comm_threads()
{
    threads_active_ = false;
    if (comm_.release_locks()) {
        input_thread_->join();
    } else {
        pthread_kill(input_thread_->native_handle(), 9);
        input_thread_->detach();
    }
    input_queue_.push({});  // release the lock
    output_queue_.push({});  // release the lock
    output_thread_->join();
}
