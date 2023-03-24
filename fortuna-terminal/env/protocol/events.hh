#ifndef EVENTS_HH_
#define EVENTS_HH_

#include <cstddef>
#include <sstream>
#include <string>
#include <unordered_map>
#include "common/syncqueue.hh"
#include "scene/ievent.hh"

class Events : public IEvent {
public:
    void event_text_input(std::string const& text) override;
    void event_key(uint8_t key, bool is_down, KeyMod mod) override;
    void event_key(SpecialKey key, bool is_down, KeyMod mod) override;
    void event_mouse_button(int button, int x, int y, bool is_down) override;
    void event_mouse_move(int button, int x, int y) override;
    void event_joystick(size_t joystick_number, size_t button, bool is_down) override;
    void event_joystick_directional(size_t joystick_number, int8_t axis, int8_t value) override;

    std::string get_lastest_events();

private:
    struct JoystickState {
        bool up = false, down = false, left = false, right = false;
    };
    std::unordered_map<size_t, JoystickState> joy_state_ {};

    static std::optional<std::string> translate_special_key(SpecialKey special_key, KeyMod mod);

    std::stringstream output_queue_;
};

#endif //EVENTS_HH_
