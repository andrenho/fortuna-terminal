#ifndef FORTUNAPROTOCOL_HH_
#define FORTUNAPROTOCOL_HH_

#include "protocol.hh"

class FortunaProtocol : public Protocol {
public:
    FortunaProtocol(OutputQueue& queue, Scene& scene) : Protocol(queue, scene) {}
};


#endif //FORTUNAPROTOCOL_HH_
