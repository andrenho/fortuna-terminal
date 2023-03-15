#ifndef IEVENT_HH_
#define IEVENT_HH_

#include <string>

enum class SpecialKey {
    ESC, F1, F2, F3, F4, F5, F6, F7, F8, F9, F10, F11, F12, TAB, CAPSLOCK, WIN,
    INSERT, HOME, END, PAGEUP, PAGEDOWN, UP, DOWN, LEFT, RIGHT, ENTER, BACKSPACE,
    DELETE, PRINTSCREEN, PAUSEBREAK,
};

struct KeyMod {
    bool shift;
    bool control;
    bool alt;
};


class IEvent {
public:
    virtual void event_text_input(std::string const& text) = 0;
    virtual void event_key(uint8_t key, bool is_down, KeyMod mod) = 0;
    virtual void event_key(SpecialKey key, bool is_down, KeyMod mod) = 0;
    virtual void event_mouse_button(int button, int x, int y, bool is_down) = 0;
    virtual void event_mouse_move(int button, int x, int y) = 0;
    virtual void event_joystick(size_t joystick_number, size_t button, bool is_down) = 0;
    virtual void event_joystick_directional(size_t joystick_number, int8_t axis, int8_t value) = 0;

protected:
    IEvent() = default;
};

#endif //IEVENT_HH_
