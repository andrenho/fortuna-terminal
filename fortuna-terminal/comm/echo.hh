#ifndef ECHO_HH_
#define ECHO_HH_

#include "comm.hh"
#include "common/syncqueue.hh"

class Echo : public CommunicationModule {
public:
    [[nodiscard]] std::vector<uint8_t> read_blocking(size_t n) override;
    void                               write(std::vector<uint8_t> const &data) override;

    void release_locks() override;

private:
    SyncQueue<uint8_t> buffer_;
};

#endif //ECHO_HH_
