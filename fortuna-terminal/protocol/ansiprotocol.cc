#include "ansiprotocol.hh"

void AnsiProtocol::input(uint8_t byte, InputQueue& input_queue) const
{
    if (byte >= 32 && byte <= 127) {
        input_queue.push({ InputEventType::PrintChar, { byte } });
    }
}

std::vector<uint8_t> AnsiProtocol::process_output_queue(OutputQueue &output_queue) const
{
    std::vector<uint8_t> r;

    std::optional<OutputEvent> o_event;
    while ((o_event = output_queue.pop())) {
        auto const& event = o_event.value();
        switch (event.type) {
            case OutputEventType::Keydown:
                // r.push_back(event.key_code.code);  // TODO - translate char
                break;
            case OutputEventType::Keyup:
                break;
            case OutputEventType::TextInput:
                for (const char* c = event.text_input; *c; ++c)
                    r.push_back(*c);
                break;
        }
    }

    return r;
}
