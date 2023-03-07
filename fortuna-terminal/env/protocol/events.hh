#ifndef EVENTS_HH_
#define EVENTS_HH_

#include <cstddef>
#include <string>
#include <unordered_map>
#include "keys.hh"

class Events {

    void event_text_input(std::string const& text);
    void event_key(uint8_t key, bool is_down, KeyMod mod);
    void event_key(SpecialKey key, bool is_down, KeyMod mod);
    void event_mouse_button(int button, int x, int y, bool is_down);
    void event_mouse_move(int button, int x, int y);
    void event_joystick(size_t joystick_number, size_t button, bool is_down);
    void event_joystick_directional(size_t joystick_number, int8_t axis, int8_t value);

    struct JoystickState {
        bool up = false, down = false, left = false, right = false;
    };
    std::unordered_map<size_t, JoystickState> joy_state_ {};
};

#endif //EVENTS_HH_
