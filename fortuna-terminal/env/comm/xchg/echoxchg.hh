#ifndef ECHOXCHG_HH_
#define ECHOXCHG_HH_

#include <queue>

#include "commexchange.hh"

class EchoXchg : public CommExchange {
public:
    uint8_t exchange(uint8_t data) override;

private:
    std::queue<uint8_t> buffer_;
};

#endif //ECHOXCHG_HH_
