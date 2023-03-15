#ifndef COMMEXCHANGE_HH_
#define COMMEXCHANGE_HH_

#include "../comm.hh"

class CommExchange : public CommunicationModule {
public:
    [[nodiscard]] Channels channels() const override { return Channels::Exchange; }

    virtual uint8_t exchange(uint8_t) = 0;
};

#endif //COMMEXCHANGE_HH_
