#ifndef COMMEXCHANGE_HH_
#define COMMEXCHANGE_HH_

#include <vector>

#include "../comm.hh"

class CommExchange : public CommunicationModule {
public:
    [[nodiscard]] Channels channels() const override { return Channels::Exchange; }
    virtual std::vector<uint8_t> exchange(std::vector<uint8_t> const& data) = 0;

    void                           execute_threads(SyncQueueByte* input_queue_, SyncQueueByte* output_queue_, bool debug_comm) override {} // TODO
    void                           finalize_threads() override {} // TODO
};

#endif //COMMEXCHANGE_HH_
