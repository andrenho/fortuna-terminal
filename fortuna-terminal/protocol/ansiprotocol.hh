#ifndef ANSIPROTOCOL_HH_
#define ANSIPROTOCOL_HH_

#include "protocol.hh"

class AnsiProtocol : public Protocol {
public:
    explicit AnsiProtocol(OutputQueue& output_queue) : Protocol(output_queue) {}

    void input_char(uint8_t byte, InputQueue& input_queue) override;

    void output_key_event(bool is_down, uint8_t key_code, KeyMod keymod) override;
    void output_special_key_event(bool is_down, SpecialKey special_key, KeyMod keymod) override;

private:
    bool escape_mode = false;
    std::string escape_sequence;

    void translate_escape_sequence(InputQueue &queue);
    void rollback_escape_sequence(InputQueue &queue);
    void parse_ansi_sequence(char command, unsigned int p1, unsigned int p2, InputQueue &input_queue);
    [[nodiscard]] uint8_t text_ansi_color(unsigned int number) const;
};

#endif //ANSIPROTOCOL_HH_
