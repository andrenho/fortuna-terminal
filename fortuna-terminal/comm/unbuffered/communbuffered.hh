#ifndef COMMUNBUFFERED_HH_
#define COMMUNBUFFERED_HH_

#include <string_view>
#include "../comm.hh"
#include "common/iterativethread.hh"

class CommUnbuffered : public CommunicationModule {
public:
    CommUnbuffered();

    std::string exchange(std::string_view data_to_send) override;

    void notify() override { thread_.notify(); }

protected:
    virtual std::string thread_exchange(std::string_view data_to_send) = 0;

    SyncQueueByte input_queue_ {},
                  output_queue_ {};
    IterativeThread thread_;
};

#endif //COMMUNBUFFERED_HH_
