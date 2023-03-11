#ifndef THREADRUNNER_HH_
#define THREADRUNNER_HH_

#include <memory>
#include <thread>

#include "common/syncqueue.hh"
#include "env/comm/comm.hh"
#include "comm/io/comm_io.hh"

class ThreadRunner {
public:
    ThreadRunner(SyncQueueByte& input_queue, SyncQueueByte& output_queue, CommunicationModule& comm)
        : input_queue_(input_queue), output_queue_(output_queue), comm_(comm) {}

    void run_comm_threads(bool debug_comm);
    void finalize_comm_threads();

private:
    bool threads_running_ = true;
    std::unique_ptr<std::thread> input_thread_ {},
                                 output_thread_ {};
    std::unique_ptr<std::mutex>  mutex_ = std::make_unique<std::mutex>();

    SyncQueueByte &input_queue_,
                  &output_queue_;
    CommunicationModule &comm_;

    void input_thread(CommIO* comm_io, bool debug_comm) const;
    void output_thread(CommIO* comm_io, bool debug_comm) const;

    void debug_byte(uint8_t byte, bool is_input) const;
};

#endif //THREADRUNNER_HH_
