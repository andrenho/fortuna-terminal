#include "fortunaprotocol.hh"

#include <vector>


void FortunaProtocol::input_char(uint8_t byte, InputQueue &input_queue)
{
    static const struct {
        FP_InputCommandType                         command;
        size_t                                      expected_size;
        std::function<InputEvent(uint8_t, uint8_t)> f;
    } commands[] = {
        { FP_TEXT_PRINT_CHAR, 2, [](uint8_t p1, uint8_t) { return InputEvent { InputEventType::TextPrintChar, p1 }; }},
        { FP_TEXT_MOVE_UP, 2, [](uint8_t p1, uint8_t) { return InputEvent { InputEventType::TextMoveUp, p1 }; } },
        { FP_TEXT_MOVE_DOWN, 2, [](uint8_t p1, uint8_t) { return InputEvent { InputEventType::TextMoveDown, p1 }; } },
        { FP_TEXT_MOVE_FORWARD, 2, [](uint8_t p1, uint8_t) { return InputEvent { InputEventType::TextMoveForward, p1 }; } },
        { FP_TEXT_MOVE_BACKWARD, 2, [](uint8_t p1, uint8_t) { return InputEvent { InputEventType::TextMoveBackward, p1 }; } },
        { FP_TEXT_MOVE_TO, 3, [](uint8_t p1, uint8_t p2) { return InputEvent { InputEventType::TextMoveUp, p1, p2 }; } },
        { FP_TEXT_RESET_FORMATTING, 1, [](uint8_t, uint8_t) { return InputEvent { InputEventType::TextResetFormatting }; } },
        { FP_TEXT_SET_COLOR, 2, [](uint8_t p1, uint8_t) { return InputEvent { InputEventType::TextSetColor, p1 }; } },
        { FP_TEXT_CLEAR_SCREEN, 1, [](uint8_t, uint8_t) { return InputEvent { InputEventType::TextClearScreen }; } },
    };

    static std::vector<uint8_t> buffer;
    buffer.push_back(byte);

    auto cmd = static_cast<FP_InputCommandType>(buffer[0]);
    size_t sz = buffer.size();

    for (auto const& command: commands) {
        if (cmd == command.command && sz == command.expected_size) {
            uint8_t p1 = sz >= 1 ? buffer.at(1) : 0;
            uint8_t p2 = sz >= 2 ? buffer.at(2) : 0;
            input_queue.enqueue(command.f(p1, p2));
            buffer.clear();
        }
    }
}

void FortunaProtocol::output_key_event(bool is_down, uint8_t key_code, KeyMod keymod)
{
    FP_OutputEvent event {};
    event.event = is_down ? FP_KEY_PRESS : FP_KEY_RELEASE;
    event.key = key_code;
    event.mod = keymod;
    for (size_t i = 0; i < sizeof(event); ++i)
        output_queue_.enqueue(*(reinterpret_cast<uint8_t*>(&event) + i));
}

void FortunaProtocol::output_special_key_event(bool is_down, SpecialKey special_key, KeyMod keymod)
{
    FP_OutputEvent event {};
    event.event = is_down ? FP_SPECIAL_KEY_PRESS : FP_SPECIAL_KEY_RELEASE;
    event.special_key = special_key;
    event.mod = keymod;
    for (size_t i = 0; i < sizeof(event); ++i)
        output_queue_.enqueue(*(reinterpret_cast<uint8_t*>(&event) + i));
}
