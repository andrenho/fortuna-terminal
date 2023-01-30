#ifndef SDLTERMINAL_HH_
#define SDLTERMINAL_HH_

#include "terminal.hh"

class SDL_Terminal : public Terminal
{
public:
    explicit SDL_Terminal(CommunicationModule& comm) : Terminal(comm) {}

    void initialize() override;
    void update() override;
};

#endif //SDLTERMINAL_HH_
