#include "ansiprotocol.hh"

#include <regex>

/***********************
 *                     *
 *        INPUT        *
 *                     *
 ***********************/

static std::regex ansi_escape_sequence_regex(R"(\[(?:(\d+)(?:;(\d+))?)?(\w))", std::regex_constants::ECMAScript);

void AnsiProtocol::input_char(uint8_t byte, InputQueue& input_queue)
{
    if (!escape_mode) {
        if (byte != '\e') {
            input_queue.enqueue({InputEventType::TextPrintChar, {byte}});
        } else {
            escape_mode = true;
            escape_sequence = "";
        }
    } else {
        escape_sequence += (char) byte;

        if (std::isalpha(byte))
            translate_escape_sequence(input_queue);
        else if (escape_sequence.size() > 16)
            rollback_escape_sequence(input_queue);
    }
}


void AnsiProtocol::translate_escape_sequence(InputQueue& input_queue)
{
    std::smatch matches;
    if(std::regex_match(escape_sequence, matches, ansi_escape_sequence_regex)) {
        unsigned int p1 = 0, p2 = 0;
        char command = matches[3].str()[0];
        if (!matches[1].str().empty())
            p1 = std::stoi(matches[1].str());
        if (!matches[2].str().empty())
            p2 = std::stoi(matches[2].str());
        parse_ansi_sequence(command, p1, p2, input_queue);
    } else {
        rollback_escape_sequence(input_queue);
    }
    escape_mode = false;
}

void AnsiProtocol::rollback_escape_sequence(InputQueue& input_queue)
{
    escape_mode = false;
    for (uint8_t byte: escape_sequence)
        input_queue.enqueue({InputEventType::TextPrintChar, byte});
}

void AnsiProtocol::parse_ansi_sequence(char command, unsigned int p1, unsigned int p2, InputQueue& input_queue)
{
    switch (command) {
        case 'A':
            input_queue.enqueue({InputEventType::TextMoveUp, (uint8_t) std::max(p1, 1U)});
            break;
        case 'B':
            input_queue.enqueue({InputEventType::TextMoveDown, (uint8_t) std::max(p1, 1U)});
            break;
        case 'C':
            input_queue.enqueue({InputEventType::TextMoveForward, (uint8_t) std::max(p1, 1U)});
            break;
        case 'D':
            input_queue.enqueue({InputEventType::TextMoveBackward, (uint8_t) std::max(p1, 1U)});
            break;
        case 'H':
            input_queue.enqueue({InputEventType::TextMoveTo, (uint8_t) std::max(p1, 1U), (uint8_t) std::max(p2, 1U)});
            break;
        case 'J':
            input_queue.enqueue(InputEvent {InputEventType::TextClearScreen});
            break;
        case 'm':
            if (p1 == 0)
                input_queue.enqueue(InputEvent {InputEventType::TextResetFormatting});
            else
                input_queue.enqueue({InputEventType::TextSetColor, text_ansi_color(p2)});
            break;
        default:
            rollback_escape_sequence(input_queue);
            break;
    }
}

uint8_t AnsiProtocol::text_ansi_color(unsigned int number) const
{
    switch (number) {
        case 30: return Color::BLACK;
        case 31: return Color::RED;
        case 32: return Color::GREEN;
        case 33: return Color::ORANGE;
        case 34: return Color::DARK_BLUE;
        case 35: return Color::PURPLE;
        case 36: return Color::TURQUOISE;
        case 37: return Color::LIGHT_GRAY;
        case 90: return Color::GRAY;
        case 91: return Color::ORANGE;
        case 92: return Color::LIME;
        case 93: return Color::YELLOW;
        case 94: return Color::LIGHT_BLUE;
        case 95: return Color::BLUE;
        case 96: return Color::CYAN;
        case 97: return Color::WHITE;
        default: return Color::WHITE;
    }
}


/***********************
 *                     *
 *       OUTPUT        *
 *                     *
 ***********************/

void AnsiProtocol::output_key_event(bool is_down, uint8_t key_code, [[maybe_unused]] KeyMod keymod)
{
    // TODO - manage key modifiers

    if (is_down) {
        output_queue_.enqueue(key_code);
    }
}

void AnsiProtocol::output_special_key_event(bool is_down, SpecialKey special_key, [[maybe_unused]] KeyMod keymod)
{
    // TODO - manage key modifiers

    if (!is_down)
        return;

    std::string str;
    switch (special_key) {
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
        output_queue_.enqueue(c);
}