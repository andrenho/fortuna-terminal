#ifndef RUNNER_HH_
#define RUNNER_HH_

#include "common/syncqueue.hh"
#include "env/comm/comm.hh"

class Runner {
public:
    Runner(SyncQueueByte& input_queue, SyncQueueByte& output_queue, CommunicationModule& comm)
        : input_queue_(input_queue), output_queue_(output_queue), comm_(comm) {}

    void run_io_threads();
    void finalize_threads();

private:
    bool threads_active_ = true;
    std::unique_ptr<std::thread> input_thread_ {}, output_thread_ {};

    SyncQueueByte &input_queue_, &output_queue_;
    CommunicationModule &comm_;
};

#endif //RUNNER_HH_
