#ifndef ECHO_HH_
#define ECHO_HH_

#include "comm.hh"

class Echo : public CommunicationModule {
public:
    [[nodiscard]] std::vector<uint8_t> read_block(size_t n) const override;
    [[nodiscard]] std::vector<uint8_t> read_for(long nanoseconds) const override;
    void                               write(std::vector<uint8_t> const &data) const override;
};

#endif //ECHO_HH_
