#ifndef COMMBUFFERED_HH_
#define COMMBUFFERED_HH_

#include <optional>
#include <queue>

#include "../comm.hh"
#include "common/iterativethread.hh"

class CommBuffered : public CommunicationModule {
public:
    CommBuffered(size_t readbuf_sz);

    [[nodiscard]] std::string exchange(std::string_view data_to_send) override;

protected:
    int                fd_ = INVALID_FILE;
    std::optional<int> write_fd_ {};

    virtual void client_disconnected();

private:
    IterativeThread output_thread_ {};
    SyncQueueByte   output_queue_ {};

    static constexpr int INVALID_FILE = -1;
    size_t readbuf_sz_;
};

#endif //COMMBUFFERED_HH_
