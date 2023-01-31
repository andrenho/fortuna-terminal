#include "ansiprotocol.hh"

void AnsiProtocol::input(uint8_t byte, InputQueue& input_queue) const
{
    switch (byte) {
        default:
            input_queue.push({InputEventType::TextPrintChar, {byte } });
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
                if (event.key_code.key_type == KeyType::Special)
                    push_special_key(event.key_code.special_key, r);
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

void AnsiProtocol::push_special_key(SpecialKey key, std::vector<uint8_t>& r) const
{
    std::string str;
    switch (key) {
        case SpecialKey::Esc: str = "\e"; break;
        case SpecialKey::F1: str = "\e[11"; break;
        case SpecialKey::F2: str = "\e[12"; break;
        case SpecialKey::F3: str = "\e[13"; break;
        case SpecialKey::F4: str = "\e[14"; break;
        case SpecialKey::F5: str = "\e[15"; break;
        case SpecialKey::F6: str = "\e[17"; break;
        case SpecialKey::F7: str = "\e[18"; break;
        case SpecialKey::F8: str = "\e[19"; break;
        case SpecialKey::F9: str = "\e[20"; break;
        case SpecialKey::F10: str = "\e[21"; break;
        case SpecialKey::F11: str = "\e[23"; break;
        case SpecialKey::F12: str = "\e[24"; break;
        case SpecialKey::Tab: str = "\t"; break;
        case SpecialKey::Insert: str = "\e[2~"; break;
        case SpecialKey::Home: str = "\e[1~"; break;
        case SpecialKey::End: str = "\e[4~"; break;
        case SpecialKey::PageUp: str = "\e[5~"; break;
        case SpecialKey::PageDown: str = "\e[6~"; break;
        case SpecialKey::Up: str = "\e[A"; break;
        case SpecialKey::Down: str = "\e[B"; break;
        case SpecialKey::Left: str = "\e[D"; break;
        case SpecialKey::Right: str = "\e[C"; break;
        case SpecialKey::Enter: str = "\n"; break;
        case SpecialKey::Backspace: str = "\b"; break;
        case SpecialKey::Delete: str = "\e[3~"; break;
        default: break;
    }
    for (uint8_t c : str)
        r.push_back(c);
}
