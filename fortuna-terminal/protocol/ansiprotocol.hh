#ifndef ANSIPROTOCOL_HH_
#define ANSIPROTOCOL_HH_

#include "protocol.hh"

class AnsiProtocol : public Protocol {
public:
    void input_char(uint8_t byte) override;

    void output_key_event(bool is_down, uint8_t key_code, KeyMod keymod) override;
    void output_special_key_event(bool is_down, SpecialKey special_key, KeyMod keymod) override;

private:
    bool escape_mode = false;
    std::string escape_sequence;

    void translate_escape_sequence();
    void rollback_escape_sequence();
    void parse_ansi_sequence(char command, unsigned int p1, unsigned int p2);
    [[nodiscard]] uint8_t text_ansi_color(unsigned int number) const;
};

#endif //ANSIPROTOCOL_HH_
