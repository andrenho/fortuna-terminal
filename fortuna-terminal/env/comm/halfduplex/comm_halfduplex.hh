#ifndef COMM_HALFDUPLEX_HH_
#define COMM_HALFDUPLEX_HH_

#include <vector>

#include "../comm.hh"
#include "common/iterativethread.hh"

class CommHalfDuplex : public CommunicationModule {
public:
    [[nodiscard]] Channels channels() const override { return Channels::Exchange; }
    virtual std::vector<uint8_t> exchange(std::vector<uint8_t> const& data) = 0;

    void execute_threads(SyncQueueByte* input_queue_, SyncQueueByte* output_queue_, bool debug_comm) override;
    void notify_threads() override;
    void finalize_threads() override;

private:
    IterativeThread thread_ {};

    void exchange_thread(SyncQueueByte* input_queue_, SyncQueueByte* output_queue_, bool debug_comm);
};

#endif //COMM_HALFDUPLEX_HH_
