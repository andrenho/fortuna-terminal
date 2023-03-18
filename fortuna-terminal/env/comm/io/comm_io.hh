#ifndef COMM_IO_HH_
#define COMM_IO_HH_

#include "env/comm/comm.hh"
#include "common/iterativethread.hh"

#include <string>
#include <optional>
#include <vector>

class CommIO : public CommunicationModule {
public:
    [[nodiscard]] Channels channels() const override { return Channels::InputAndOutput; }

    virtual std::vector<uint8_t>   read_blocking(size_t n) = 0;
    virtual std::optional<uint8_t> read_blocking();

    virtual void                   write(std::vector<uint8_t> const& data) = 0;
    void                           write(std::string const& str);
    void                           write(uint8_t byte);

    void                           execute_threads(SyncQueueByte* input_queue_, SyncQueueByte* output_queue_, bool debug_comm) override;
    void                           finalize_threads() override;

private:
    IterativeThread input_thread_ {},
                    output_thread_ {};

    void input_thread(SyncQueueByte* input_queue_, bool debug_comm);
    void output_thread(SyncQueueByte* output_queue_, bool debug_comm);
};

#endif //COMM_IO_HH_
