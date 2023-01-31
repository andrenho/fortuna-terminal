#ifndef ANSIPROTOCOL_HH_
#define ANSIPROTOCOL_HH_

#include "protocol.hh"

class AnsiProtocol : public Protocol {
public:
    void input(uint8_t byte, InputQueue& input_queue) const override;
    std::vector<uint8_t> process_output_queue(OutputQueue &output_queue) const override;

    void push_special_key(SpecialKey key, std::vector<uint8_t>& r) const;
};

#endif //ANSIPROTOCOL_HH_
