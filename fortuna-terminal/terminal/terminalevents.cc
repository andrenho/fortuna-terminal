#include "terminalevents.hh"
#include "../common/enums/execution.hh"

#define JOY_THRESHOLD 1000

#include <cstring>

ExecutionStatus TerminalEvents::process_user_events(IEvent& events) const
{
    SDL_Event e;
    while (SDL_PollEvent(&e)) {
        if (e.type == SDL_QUIT || (e.type == SDL_KEYDOWN && e.key.keysym.sym == SDLK_F12 && e.key.keysym.mod & KMOD_CTRL))
            return ExecutionStatus::Quit;

        else if (e.type == SDL_TEXTINPUT) {
            if (joystick_emulation_ && strlen(e.text.text) == 1 && strchr(emulated_keys, e.text.text[0]) != nullptr)
                break;
            events.event_text_input(e.text.text);
        }

        else if (e.type == SDL_KEYDOWN) {
            add_keyboard_event(events, true, e.key);
        }

        else if (e.type == SDL_KEYUP) {
            add_keyboard_event(events, false, e.key);
        }

        else if ((e.type == SDL_MOUSEBUTTONDOWN || e.type == SDL_MOUSEBUTTONUP) && mouse_active_) {
            events.event_mouse_button(e.button.button, e.button.x, e.button.y, e.type == SDL_MOUSEBUTTONDOWN);
        }

        else if (e.type == SDL_MOUSEMOTION && mouse_active_ && mouse_register_move_) {
            int button = 0;
            if (e.motion.state & SDL_BUTTON_MMASK)
                button = 2;
            else if (e.motion.state & SDL_BUTTON_RMASK)
                button = 3;
            else if (e.motion.state & SDL_BUTTON_LMASK)
                button = 1;
            events.event_mouse_move(button, e.motion.x, e.motion.y);
        }

        else if (e.type == SDL_JOYDEVICEADDED || e.type == SDL_JOYDEVICEREMOVED) {
            SDL_JoystickOpen(e.jdevice.which);
        }

        else if (e.type == SDL_JOYBUTTONDOWN || e.type == SDL_JOYBUTTONUP) {
            events.event_joystick(e.jdevice.which, e.jbutton.button, e.type == SDL_JOYBUTTONDOWN);
        }

        else if (e.type == SDL_JOYAXISMOTION) {
            int8_t value = 0;
            if (e.jaxis.value > JOY_THRESHOLD)
                value = 1;
            else if (e.jaxis.value < -JOY_THRESHOLD)
                value = -1;
            events.event_joystick_directional(e.jaxis.which, e.jaxis.axis, value);
        }
    }

    return ExecutionStatus::Continue;
}

ExecutionStatus TerminalEvents::wait_for_enter() const
{
    while (true) {
        SDL_Event e;
        while (SDL_PollEvent(&e)) {
            if (e.type == SDL_QUIT || (e.type == SDL_KEYDOWN && e.key.keysym.sym == SDLK_F12 && e.key.keysym.mod & KMOD_CTRL)) {
                return ExecutionStatus::Quit;
            } else if (e.type == SDL_KEYDOWN && (e.key.keysym.sym == SDLK_RETURN || e.key.keysym.sym == SDLK_KP_ENTER)) {
                return ExecutionStatus::Continue;
            }
        }
    }
}


void TerminalEvents::add_keyboard_event(IEvent& events, bool is_down, SDL_KeyboardEvent key) const
{
    if (joystick_emulation_) {
        if (key.repeat != 0)
            return;
        switch (key.keysym.sym) {
            case SDLK_UP: events.event_joystick_directional(0, 1, is_down ? -1 : 0); return;
            case SDLK_DOWN: events.event_joystick_directional(0, 1, is_down ? 1 : 0); return;
            case SDLK_LEFT: events.event_joystick_directional(0, 0, is_down ? -1 : 0); return;
            case SDLK_RIGHT: events.event_joystick_directional(0, 0, is_down ? 1 : 0); return;
            case SDLK_x: events.event_joystick(0, 1, is_down); return;
            case SDLK_z: events.event_joystick(0, 2, is_down); return;
            case SDLK_s: events.event_joystick(0, 0, is_down); return;
            case SDLK_a: events.event_joystick(0, 3, is_down); return;
            case SDLK_RETURN: events.event_joystick(0, 9, is_down); return;
            case SDLK_TAB: events.event_joystick(0, 8, is_down); return;
            case SDLK_q: events.event_joystick(0, 4, is_down); return;
            case SDLK_w: events.event_joystick(0, 5, is_down); return;
        }
    }

    KeyMod key_modifiers {
            (key.keysym.mod & KMOD_SHIFT) != 0,
            (key.keysym.mod & KMOD_CTRL) != 0,
            (key.keysym.mod & KMOD_ALT) != 0,
    };

    if (key.keysym.sym >= 32 && key.keysym.sym < 127) {
        events.event_key((uint8_t) key.keysym.sym, is_down, key_modifiers);
    } else {
        SpecialKey special_key;

        switch (key.keysym.sym) {
            case SDLK_ESCAPE: special_key = SpecialKey::ESC; break;
            case SDLK_F1: special_key = SpecialKey::F1; break;
            case SDLK_F2: special_key = SpecialKey::F2; break;
            case SDLK_F3: special_key = SpecialKey::F3; break;
            case SDLK_F4: special_key = SpecialKey::F4; break;
            case SDLK_F5: special_key = SpecialKey::F5; break;
            case SDLK_F6: special_key = SpecialKey::F6; break;
            case SDLK_F7: special_key = SpecialKey::F7; break;
            case SDLK_F8: special_key = SpecialKey::F8; break;
            case SDLK_F9: special_key = SpecialKey::F9; break;
            case SDLK_F10: special_key = SpecialKey::F10; break;
            case SDLK_F11: special_key = SpecialKey::F11; break;
            case SDLK_F12: special_key = SpecialKey::F12; break;
            case SDLK_TAB: special_key = SpecialKey::TAB; break;
            case SDLK_CAPSLOCK: special_key = SpecialKey::CAPSLOCK; break;
            case SDLK_APPLICATION: special_key = SpecialKey::WIN; break;
            case SDLK_INSERT: special_key = SpecialKey::INSERT; break;
            case SDLK_HOME: special_key = SpecialKey::HOME; break;
            case SDLK_END: special_key = SpecialKey::END; break;
            case SDLK_PAGEUP: special_key = SpecialKey::PAGEUP; break;
            case SDLK_PAGEDOWN: special_key = SpecialKey::PAGEDOWN; break;
            case SDLK_UP: special_key = SpecialKey::UP; break;
            case SDLK_DOWN: special_key = SpecialKey::DOWN; break;
            case SDLK_LEFT: special_key = SpecialKey::LEFT; break;
            case SDLK_RIGHT: special_key = SpecialKey::RIGHT; break;
            case SDLK_RETURN: case SDLK_KP_ENTER: special_key = SpecialKey::ENTER; break;
            case SDLK_BACKSPACE: special_key = SpecialKey::BACKSPACE; break;
            case SDLK_DELETE: special_key = SpecialKey::DELETE; break;
            case SDLK_PRINTSCREEN: special_key = SpecialKey::PRINTSCREEN; break;
            case SDLK_PAUSE: special_key = SpecialKey::PAUSEBREAK; break;
            default: return;
        }
        events.event_key(special_key, is_down, key_modifiers);
    }
}

void TerminalEvents::set_mouse_active(bool value)
{
    mouse_active_ = value;
    SDL_ShowCursor(mouse_active_ ? SDL_ENABLE : SDL_DISABLE);
}
