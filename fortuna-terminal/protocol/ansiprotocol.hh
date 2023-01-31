#ifndef ANSIPROTOCOL_HH_
#define ANSIPROTOCOL_HH_

#include "protocol.hh"

class AnsiProtocol : public Protocol {
public:
    void input(uint8_t byte, InputQueue& input_queue) override;
    std::vector<uint8_t> process_output_queue(OutputQueue &output_queue) const override;

    void push_special_key(SpecialKey key, std::vector<uint8_t>& r) const;

private:
    bool escape_mode = false;
    std::string escape_sequence;

    void translate_escape_sequence(InputQueue &queue);
    void rollback_escape_sequence(InputQueue &queue);
    void parse_ansi_sequence(char command, unsigned int p1, unsigned int p2, InputQueue &input_queue);
    uint8_t text_ansi_color(int number) const;
};

#endif //ANSIPROTOCOL_HH_
