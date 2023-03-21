#ifndef ECHO_HH_
#define ECHO_HH_

#include "comm_fullduplex.hh"
#include "common/syncqueue.hh"

#include <vector>

class Echo : public CommFullDuplex {
public:
    bool                 release_locks() override;

    std::string description() const override { return "Echo (full duplex)"; }

protected:
    std::vector<uint8_t> read_for(Duration duration) override;
    std::vector<uint8_t> read_blocking(size_t n) override;
    void                 write(std::vector<uint8_t> const &data) override;

private:
    SyncQueue<uint8_t> buffer_;
};

#endif //ECHO_HH_
