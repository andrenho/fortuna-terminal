#ifndef RUNNER_HH_
#define RUNNER_HH_

#include "common/syncqueue.hh"
#include "env/comm/comm.hh"

class Runner {
public:
    void run_io_threads(SyncQueueByte &input_queue, SyncQueueByte &output_queue, CommunicationModule &comm);

private:
    bool threads_active_ = true;
    std::unique_ptr<std::thread> input_thread_ {}, output_thread_ {};
};

#endif //RUNNER_HH_
