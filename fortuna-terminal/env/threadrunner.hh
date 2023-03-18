#ifndef THREADRUNNER_HH_
#define THREADRUNNER_HH_

#include <memory>
#include <thread>

#include "common/iterativethread.hh"
#include "common/syncqueue.hh"
#include "env/comm/comm.hh"
#include "comm/io/comm_io.hh"
#include "env/comm/xchg/commexchange.hh"

class ThreadRunner {
public:
    ThreadRunner(SyncQueueByte& input_queue, SyncQueueByte& output_queue, CommunicationModule& comm)
        : input_queue_(input_queue), output_queue_(output_queue), comm_(comm) {}

    void run_comm_threads(bool debug_comm);
    void finalize_comm_threads();

    void notify_exchange_thread();

private:
    std::thread input_thread_ {},
                output_thread_ {};
    IterativeThread exchange_thread_ {};

    std::mutex debug_mutex_ {};

    bool threads_running_ = true;

    SyncQueueByte &input_queue_,
                  &output_queue_;
    CommunicationModule &comm_;

    void input_thread(CommIO* comm_io, bool debug_comm);
    void output_thread(CommIO* comm_io, bool debug_comm);
    void exchange_thread(CommExchange* comm_xchg, bool debug_comm);

    void debug_byte(uint8_t byte, bool is_input);
};

#endif //THREADRUNNER_HH_
