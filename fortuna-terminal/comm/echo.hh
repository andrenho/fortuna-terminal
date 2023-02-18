#ifndef ECHO_HH_
#define ECHO_HH_

#include "comm.hh"
#include "common/syncqueue.hh"

class Echo : public CommunicationModule {
public:
    [[nodiscard]] std::vector<uint8_t> read_blocking(size_t n) override;
    [[nodiscard]] std::vector<uint8_t> read_for(std::chrono::duration<double> duration) override;
    void                               write(std::vector<uint8_t> const &data) override;

private:
    SyncQueue<uint8_t> buffer_;
};

#endif //ECHO_HH_
