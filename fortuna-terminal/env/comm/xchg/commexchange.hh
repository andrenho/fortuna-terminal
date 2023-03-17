#ifndef COMMEXCHANGE_HH_
#define COMMEXCHANGE_HH_

#include <vector>

#include "../comm.hh"

class CommExchange : public CommunicationModule {
public:
    [[nodiscard]] Channels channels() const override { return Channels::Exchange; }

    [[nodiscard]] virtual std::vector<uint8_t> exchange(std::vector<uint8_t> const& data) const = 0;
};

#endif //COMMEXCHANGE_HH_
