#ifndef PROTOCOL_HH_
#define PROTOCOL_HH_

#include <string>

#include "ansi.hh"
#include "common/enums/mode.hh"
#include "common/types/time.hh"
#include "fortuna.hh"
#include "events.hh"

class Protocol : public IEvent {
public:
    explicit Protocol(class Scene& scene);

    void                 execute_inputs(std::vector<uint8_t> const& data_received);
    std::vector<uint8_t> execute_outputs();

    void reset();
    void reset_mode();

    void event_text_input(std::string const& text) override;
    void event_key(uint8_t key, bool is_down, KeyMod mod) override;
    void event_key(SpecialKey key, bool is_down, KeyMod mod) override;
    void event_mouse_button(int button, int x, int y, bool is_down) override;
    void event_mouse_move(int button, int x, int y) override;
    void event_joystick(size_t joystick_number, size_t button, bool is_down) override;
    void event_joystick_directional(size_t joystick_number, int8_t axis, int8_t value) override;

private:
    ANSI            ansi_;
    FortunaProtocol fortuna_;
    Scene const&    scene_;
};

#endif //PROTOCOL_HH_
