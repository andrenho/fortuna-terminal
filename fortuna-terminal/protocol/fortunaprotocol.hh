#ifndef FORTUNAPROTOCOL_HH_
#define FORTUNAPROTOCOL_HH_

#include "protocol.hh"
#include "../event/inputevent.hh"

class FortunaProtocol : public Protocol {
public:
    void input_char(uint8_t byte) override;
    void output_key_event(bool is_down, uint8_t key_code, KeyMod keymod) override;
    void output_special_key_event(bool is_down, SpecialKey special_key, KeyMod keymod) override;

private:
    void protocol_error(FP_Response error_code, std::vector<uint8_t> const& buffer);
};

#endif //FORTUNAPROTOCOL_HH_
