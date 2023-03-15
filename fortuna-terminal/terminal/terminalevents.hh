#ifndef TERMINALEVENTS_HH_
#define TERMINALEVENTS_HH_

#include <SDL2/SDL.h>

#include "common/enums/execution.hh"
#include "env/protocol/events.hh"

class TerminalEvents {
public:
    ExecutionStatus process_user_events(IEvent& events) const;
    ExecutionStatus wait_for_enter() const;

    void            set_mouse_active(bool value);
    void            set_mouse_register_move(bool mouse_register_move) { mouse_register_move_ = mouse_register_move; }
    void            set_joystick_emulation(bool value) { joystick_emulation_ = value; }

private:
    void add_keyboard_event(IEvent& events, bool is_down, SDL_KeyboardEvent key) const;

    bool mouse_active_ = false;
    bool mouse_register_move_ = false;
    bool joystick_emulation_ = false;

    static constexpr const char* emulated_keys = "XxZzSsAaQqWw\r\n\t";
};

#endif //TERMINALEVENTS_HH_
