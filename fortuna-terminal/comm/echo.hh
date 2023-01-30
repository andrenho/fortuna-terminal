#ifndef ECHO_HH_
#define ECHO_HH_

#include "comm_module.hh"

class Echo : public CommunicationModule {
public:
    void run(Protocol* protocol) override;
};

#endif //ECHO_HH_
