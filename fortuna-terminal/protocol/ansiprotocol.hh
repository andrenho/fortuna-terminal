#ifndef ANSIPROTOCOL_HH_
#define ANSIPROTOCOL_HH_

#include "protocol.hh"

class AnsiProtocol : public Protocol {
public:
    AnsiProtocol(OutputQueue& queue, Scene& scene) : Protocol(queue, scene) {}
};

#endif //ANSIPROTOCOL_HH_
