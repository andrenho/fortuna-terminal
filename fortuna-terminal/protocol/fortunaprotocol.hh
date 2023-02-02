#ifndef FORTUNAPROTOCOL_HH_
#define FORTUNAPROTOCOL_HH_

#include "protocol.hh"
#include "../event/inputevent.hh"

class FortunaProtocol : public Protocol {
public:
    explicit FortunaProtocol(OutputQueue& output_queue) : Protocol(output_queue) {}

    void input_char(uint8_t byte, InputQueue &input_queue) override;
    void output_key_event(bool is_down, uint8_t key_code, KeyMod keymod) override;
    void output_special_key_event(bool is_down, SpecialKey special_key, KeyMod keymod) override;
};

#endif //FORTUNAPROTOCOL_HH_
