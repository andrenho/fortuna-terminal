#include "ansiprotocol.hh"

void AnsiProtocol::input(uint8_t byte)
{
    if (byte >= 32 && byte <= 127) {
        input_queue_.push({ InputEventType::PrintChar, { byte } });
    }
}
