#ifndef ECHO_HH_
#define ECHO_HH_

#include "comm.hh"
#include "old/common/syncqueue.hh"

class Echo : public CommunicationModule {
public:
    std::vector<uint8_t> read_blocking(size_t n) override;
    void                 write(std::vector<uint8_t> const &data) override;

    bool                 release_locks() override;

private:
    SyncQueue<uint8_t> buffer_;
};

#endif //ECHO_HH_