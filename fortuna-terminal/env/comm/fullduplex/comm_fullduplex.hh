#ifndef COMM_FULLDUPLEX_HH_
#define COMM_FULLDUPLEX_HH_

#include "comm/comm.hh"
#include "common/iterativethread.hh"

#include <atomic>
#include <string>
#include <optional>
#include <vector>

class CommFullDuplex : public CommunicationModule {
public:
    void execute_threads(SyncQueueByte* input_queue_, SyncQueueByte* output_queue_, bool debug_comm) override;
    void finalize_threads() override;

    [[nodiscard]] bool is_overwhelmed() const override { return is_overwhelmed_; }

private:
    virtual std::vector<uint8_t>   read_for(Duration duration) = 0;

    virtual void                   write(std::vector<uint8_t> const& data) = 0;
    void                           write(std::string const& str);
    void                           write(uint8_t byte);

    IterativeThread input_thread_ {},
                    output_thread_ {};

    void input_thread(SyncQueueByte* input_queue_, bool debug_comm);
    void output_thread(SyncQueueByte* output_queue_, bool debug_comm);

    std::atomic_bool is_overwhelmed_ = false;
};

#endif //COMM_FULLDUPLEX_HH_
