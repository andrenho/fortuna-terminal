#include "events.hh"

#include <optional>
#include <vector>

using namespace std::string_literals;

void Events::event_text_input(std::string const &text)
{
    std::vector<uint8_t> v(text.begin(), text.end());
    output_queue_.push_all(v);
}

void Events::event_key(uint8_t key, bool is_down, KeyMod mod)
{
    if (is_down) {
        if (mod.control) {
            if (key >= 'a' && key <= 'z')
                output_queue_.push(key - 96);
        }
    }
}

void Events::event_key(SpecialKey key, bool is_down, KeyMod mod)
{
    if (is_down) {
        auto ostr = translate_special_key(key, mod);
        if (ostr.has_value()) {
            std::vector<uint8_t> v(ostr.value().begin(), ostr.value().end());
            output_queue_.push_all(v);
        }
    }
}

void Events::event_mouse_button(int button, int x, int y, bool is_down)
{
    std::string s = "\e#"s + std::to_string(button) + ";" + std::to_string(x) + ";" + std::to_string(y) +
                    ";" + (is_down ? "B" : "R");
    output_queue_.push_all(s);
}

void Events::event_mouse_move(int button, int x, int y)
{
    output_queue_.push_all("\e#"s + std::to_string(button) + ";" + std::to_string(x) + ";" + std::to_string(y) + "M");
}

void Events::event_joystick(size_t joystick_number, size_t button, bool is_down)
{
    output_queue_.push_all("\e#"s + std::to_string(joystick_number) + ";" + std::to_string(button) + (is_down ? 'J' : 'K' ));
}

void Events::event_joystick_directional(size_t joystick_number, int8_t axis, int8_t value)
{
    JoystickState& state = joy_state_[joystick_number];
    JoystickState new_state = state;

    if (axis == 1) {
        new_state.up = (value == -1);
        new_state.down = (value == 1);
    } else {
        new_state.left = (value == -1);
        new_state.right = (value == 1);
    }

    if (state.up != new_state.up)
        output_queue_.push_all("\e#"s + std::to_string(joystick_number) + ";18" + (new_state.up ? 'J' : 'K'));
    if (state.down != new_state.down)
        output_queue_.push_all("\e#"s + std::to_string(joystick_number) + ";19" + (new_state.down ? 'J' : 'K'));
    if (state.left != new_state.left)
        output_queue_.push_all("\e#"s + std::to_string(joystick_number) + ";16" + (new_state.left ? 'J' : 'K'));
    if (state.right != new_state.right)
        output_queue_.push_all("\e#"s + std::to_string(joystick_number) + ";17" + (new_state.right ? 'J' : 'K'));

    joy_state_[joystick_number] = new_state;
}

std::optional<std::string> Events::translate_special_key(SpecialKey special_key, KeyMod mod)
{
    (void) mod;

    switch (special_key) {
        case SpecialKey::ESC:        return "\e";
        case SpecialKey::ENTER:      return "\r";
        case SpecialKey::TAB:        return "\t";
        case SpecialKey::BACKSPACE:  return "\b";
        case SpecialKey::F1:         return "\e[11~";
        case SpecialKey::F2:         return "\e[12~";
        case SpecialKey::F3:         return "\e[13~";
        case SpecialKey::F4:         return "\e[14~";
        case SpecialKey::F5:         return "\e[15~";
        case SpecialKey::F6:         return "\e[17~";
        case SpecialKey::F7:         return "\e[18~";
        case SpecialKey::F8:         return "\e[19~";
        case SpecialKey::F9:         return "\e[20~";
        case SpecialKey::F10:        return "\e[21~";
        case SpecialKey::F11:        return "\e[23~";
        case SpecialKey::F12:        return "\e[24~";
        case SpecialKey::INSERT:     return "\e[2~";
        case SpecialKey::HOME:       return "\e[1~";
        case SpecialKey::END:        return "\e[4~";
        case SpecialKey::PAGEUP:     return "\e[5~";
        case SpecialKey::PAGEDOWN:   return "\e[6~";
        case SpecialKey::UP:         return "\e[A";
        case SpecialKey::DOWN:       return "\e[B";
        case SpecialKey::LEFT:       return "\e[D";
        case SpecialKey::RIGHT:      return "\e[C";
        case SpecialKey::DELETE:     return "\e[3~";
        case SpecialKey::PRINTSCREEN:
        case SpecialKey::PAUSEBREAK:
        case SpecialKey::CAPSLOCK:
        case SpecialKey::WIN:
            break;
    }
    return {};
}
