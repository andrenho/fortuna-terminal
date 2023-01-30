#ifndef ECHO_HH_
#define ECHO_HH_

#include "comm_module.hh"

class Echo : public CommunicationModule {
public:
    explicit Echo(Protocol const* protocol) : CommunicationModule(protocol) {}
};

#endif //ECHO_HH_
