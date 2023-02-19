#ifndef ECHOFORTUNA_HH_
#define ECHOFORTUNA_HH_

#include "comm.hh"

class EchoFortuna : public CommunicationModule {
public:
    std::vector<uint8_t> read_blocking(size_t n) override;
    void                 write(std::vector<uint8_t> const &data) override;

    bool                 release_locks() override;
};

#endif //ECHOFORTUNA_HH_
