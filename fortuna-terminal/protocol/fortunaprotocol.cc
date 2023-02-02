#include "fortunaprotocol.hh"
#include "global.hh"

#include <functional>
#include <vector>
#include <iomanip>

static const struct {
    FP_InputCommandType                         command;
    size_t                                      expected_size;
    std::function<InputEvent(uint8_t, uint8_t)> f;
} commands[] = {
        { FP_TEXT_PRINT_CHAR, 2, [](uint8_t p1, uint8_t) {
            return InputEvent { InputEventType::TextPrintChar, p1 };
        }},
        { FP_TEXT_MOVE_UP, 2, [](uint8_t p1, uint8_t) {
            return InputEvent { InputEventType::TextMoveUp, p1 };
        } },
        { FP_TEXT_MOVE_DOWN, 2, [](uint8_t p1, uint8_t) {
            return InputEvent { InputEventType::TextMoveDown, p1 };
        } },
        { FP_TEXT_MOVE_FORWARD, 2, [](uint8_t p1, uint8_t) {
            return InputEvent { InputEventType::TextMoveForward, p1 };
        } },
        { FP_TEXT_MOVE_BACKWARD, 2, [](uint8_t p1, uint8_t) {
            return InputEvent { InputEventType::TextMoveBackward, p1 };
        } },
        { FP_TEXT_MOVE_TO, 3, [](uint8_t p1, uint8_t p2) {
            return InputEvent { InputEventType::TextMoveUp, p1, p2 };
        } },
        { FP_TEXT_RESET_FORMATTING, 1, [](uint8_t, uint8_t) {
            return InputEvent { InputEventType::TextResetFormatting };
        } },
        { FP_TEXT_SET_COLOR, 2, [](uint8_t p1, uint8_t) {
            return InputEvent { InputEventType::TextSetColor, p1 };
        } },
        { FP_TEXT_CLEAR_SCREEN, 1, [](uint8_t, uint8_t) {
            return InputEvent { InputEventType::TextClearScreen };
        } },
        { FP_TEXT_CLEAR_TO_BOL, 1, [](uint8_t, uint8_t) {
            return InputEvent { InputEventType::TextClearToBeginningOfLine };
        } },
        { FP_TEXT_CLEAR_TO_EOL, 1, [](uint8_t, uint8_t) {
            return InputEvent { InputEventType::TextClearToEndOfLine };
        } },
        { FP_TEXT_CLEAR_LINE, 1, [](uint8_t, uint8_t) {
            return InputEvent { InputEventType::TextClearLine };
        } },
        { FP_TEXT_SET_SCROLL_REGION, 1, [](uint8_t p1, uint8_t p2) {
            return InputEvent { InputEventType::SetScrollRegion, p1, p2 };
        } },
        { FP_TEXT_DEL_CHAR_UNDER_CURSOR, 1, [](uint8_t, uint8_t) {
            return InputEvent { InputEventType::DeleteCharUnderCursor };
        } }
};

void FortunaProtocol::input_char(uint8_t byte)
{
    static std::vector<uint8_t> buffer;
    buffer.push_back(byte);

    auto cmd = static_cast<FP_InputCommandType>(buffer[0]);
    size_t sz = buffer.size();

    // Check buffered input for valid commands, and execute them
    for (auto const& command: commands) {
        if (cmd == command.command && sz == command.expected_size) {
            uint8_t p1 = sz > 1 ? buffer.at(1) : 0;
            uint8_t p2 = sz > 2 ? buffer.at(2) : 0;
            input_queue.enqueue(command.f(p1, p2));
            buffer.clear();
            return;
        }
    }

    // If we reached this point, it means there was no match with valid commands. Check if the initial byte was bogus.
    for (auto const& command: commands) {
        if (buffer[0] == command.command)  // this means we're still waiting for completion of command
            return;
    }
    protocol_error(FP_INVALID_COMMAND, buffer);
    buffer.clear();
}

void FortunaProtocol::output_key_event(bool is_down, uint8_t key_code, KeyMod keymod)
{
    FP_OutputEvent event {};
    event.event = is_down ? FP_KEY_PRESS : FP_KEY_RELEASE;
    event.key = key_code;
    event.mod = keymod;
    for (size_t i = 0; i < sizeof(event); ++i)
        output_queue.enqueue(*(reinterpret_cast<uint8_t*>(&event) + i));
}

void FortunaProtocol::output_special_key_event(bool is_down, SpecialKey special_key, KeyMod keymod)
{
    FP_OutputEvent event {};
    event.event = is_down ? FP_SPECIAL_KEY_PRESS : FP_SPECIAL_KEY_RELEASE;
    event.special_key = special_key;
    event.mod = keymod;
    for (size_t i = 0; i < sizeof(event); ++i)
        output_queue.enqueue(*(reinterpret_cast<uint8_t*>(&event) + i));
}

void FortunaProtocol::protocol_error(FP_Response error_code, std::vector<uint8_t> const& buffer)
{
    if (options.debug_mode) {
        std::cerr << "Communication error, response code "
                  << std::hex << std::setw(2) << std::setfill('0') << (int) error_code << " (buffer:";
        for (uint8_t byte: buffer)
            std::cerr << " " << std::hex << std::setw(2) << std::setfill('0') << (int) byte;
        std::cerr << ")";
    }
}
