#ifndef ANSIPROTOCOL_HH_
#define ANSIPROTOCOL_HH_

#include "protocol.hh"

class AnsiProtocol : public Protocol {
public:
    AnsiProtocol(InputQueue& input_queue, OutputQueue &output_queue) : Protocol(input_queue, output_queue) {}

    void input(uint8_t byte) override;
};

#endif //ANSIPROTOCOL_HH_
