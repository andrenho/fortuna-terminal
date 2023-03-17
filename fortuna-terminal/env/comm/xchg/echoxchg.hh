#ifndef ECHOXCHG_HH_
#define ECHOXCHG_HH_

#include <queue>

#include "commexchange.hh"

class EchoXchg : public CommExchange {
public:
    std::vector<uint8_t> exchange(std::vector<uint8_t> const &data) override;
};

#endif //ECHOXCHG_HH_
