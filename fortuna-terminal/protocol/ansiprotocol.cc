#include "ansiprotocol.hh"

#include <regex>

#include "global.hh"

/***********************
 *                     *
 *        INPUT        *
 *                     *
 ***********************/

static std::regex ansi_escape_sequence_regex(R"(\[\??(?:(\d+)(?:;(\d+))*)?(\w))", std::regex_constants::ECMAScript);

void AnsiProtocol::input_char(uint8_t byte)
{
    if (!escape_mode) {
        if (byte != '\e') {
            if (byte == 7)
                input_queue.enqueue(InputEvent {InputEventType::Beep });
            else if (byte == 8)
                input_queue.enqueue(InputEvent { InputEventType::TextMoveBackward, 1 });
            else if ((byte == 10 || byte >= 32))
                input_queue.enqueue({InputEventType::TextPrintChar, byte});
        } else {
            escape_mode = true;
            escape_sequence = "";
        }
    } else {
        if (byte == '\e') {
            rollback_escape_sequence();
            escape_mode = true;
            escape_sequence = "";
            return;
        }

        escape_sequence += (char) byte;

        if (std::isalpha(byte))
            translate_escape_sequence();
        else if (escape_sequence.size() > 16)
            rollback_escape_sequence();
    }
}


void AnsiProtocol::translate_escape_sequence()
{
    std::smatch matches;
    if(std::regex_match(escape_sequence, matches, ansi_escape_sequence_regex)) {
        unsigned int p1 = 0, p2 = 0;
        char command = matches[3].str()[0];
        if (!matches[1].str().empty())
            p1 = std::stoi(matches[1].str());
        if (!matches[2].str().empty())
            p2 = std::stoi(matches[2].str());
        if (escape_sequence.size() > 2 && escape_sequence[1] == '?')
            parse_custom_ansi_sequence(command, p1, p2);
        else
            parse_ansi_sequence(command, p1, p2);
    } else {
        rollback_escape_sequence();
    }
    escape_mode = false;
}

void AnsiProtocol::rollback_escape_sequence()
{
    escape_mode = false;
    if (escape_sequence.size() >= 2 && escape_sequence[1] == '?') {
        if (options.debug_mode)
            std::cerr << "Unsupported ANSI sequence received: '^" << escape_sequence << "'.\n";
    } else {
        for (uint8_t byte: escape_sequence)
            input_queue.enqueue({InputEventType::TextPrintChar, byte});
        if (options.debug_mode)
            std::cerr << "Invalid ANSI sequence received: '^" << escape_sequence << "'.\n";
    }
}

void AnsiProtocol::parse_ansi_sequence(char command, unsigned int p1, unsigned int p2)
{
    switch (command) {
        case 'A':
            input_queue.enqueue({InputEventType::TextMoveUp, (uint8_t) std::max(p1, 1U)});
            debug_special("TextMoveUp", p1);
            break;
        case 'B':
            input_queue.enqueue({InputEventType::TextMoveDown, (uint8_t) std::max(p1, 1U)});
            debug_special("TextMoveDown", p1);
            break;
        case 'C':
            input_queue.enqueue({InputEventType::TextMoveForward, (uint8_t) std::max(p1, 1U)});
            debug_special("TextMoveForward", p1);
            break;
        case 'D':
            input_queue.enqueue({InputEventType::TextMoveBackward, (uint8_t) std::max(p1, 1U)});
            debug_special("TextMoveBackwards", p1);
            break;
        case 'H':
            input_queue.enqueue({InputEventType::TextMoveTo, (uint8_t) std::max(p1, 1U), (uint8_t) std::max(p2, 1U)});
            debug_special("TextMoveTo", p1, p2);
            break;
        case 'h':
            if (p1 == 4) {
                input_queue.enqueue({ InputEventType::SetInsertionMode, 1 });
                debug_special("SetInsertionMode", 1);
            } else {
                rollback_escape_sequence();
            }
            break;
        case 'J':
            input_queue.enqueue(InputEvent {InputEventType::TextClearScreen});
            debug_special("TextClearScreen");
            break;
        case 'K':
            if (p1 == 0) {
                input_queue.enqueue(InputEvent { InputEventType::TextClearToEndOfLine });
                debug_special("TextClearToEndOfLine");
            } else if (p1 == 1) {
                input_queue.enqueue(InputEvent { InputEventType::TextClearToBeginningOfLine });
                debug_special("TextClearToBeginningOfLine");
            } else if (p1 == 2) {
                input_queue.enqueue(InputEvent { InputEventType::TextClearLine });
                debug_special("TextClearLine");
            } else {
                rollback_escape_sequence();
            }
            break;
        case 'l':
            if (p1 == 4) {
                input_queue.enqueue({ InputEventType::SetInsertionMode, 0 });
                debug_special("SetInsertionMode", 0);
            } else {
                rollback_escape_sequence();
            }
            break;
        case 'P':
            if (p1 == 0) {
                input_queue.enqueue(InputEvent { InputEventType::DeleteCharUnderCursor });
                debug_special("DeleteCharUnderCursor");
            } else {
                rollback_escape_sequence();
            }
            break;
        case 'm':
            if (p1 == 0) {
                input_queue.enqueue(InputEvent {InputEventType::TextResetFormatting});
                debug_special("TextResetFormatting");
            } else {
                input_queue.enqueue({InputEventType::TextSetColor, text_ansi_color(p2)});
                debug_special("TextSetColor", p2);
            }
            break;
        case 'n':
            if (p1 == 6) {
                std::string response = "\e[" + std::to_string(scene.text_layer.cursor().y + 1) + ";"
                        + std::to_string(scene.text_layer.cursor().x + 1) + "R";
                for (char c: response)
                    output_queue.enqueue(c);
            } else {
                rollback_escape_sequence();
            }
            break;
        case 'r':
            input_queue.enqueue({InputEventType::SetScrollRegion, (uint8_t) p1, (uint8_t) p2});
            debug_special("SetScrollRegion", p1, p2);
            break;
        default:
            rollback_escape_sequence();
            break;
    }
}

void AnsiProtocol::parse_custom_ansi_sequence(char command, unsigned int p1, unsigned int)
{
    switch (command) {
        case 'l':
            if (p1 == 2004)
                /* ignore */;
            else
                rollback_escape_sequence();
            break;
        case 'h':
            if (p1 == 2004)
                /* ignore */;
            else
                rollback_escape_sequence();
            break;
        default:
            rollback_escape_sequence();
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
        output_queue.enqueue(key_code);
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
        output_queue.enqueue(c);
}

