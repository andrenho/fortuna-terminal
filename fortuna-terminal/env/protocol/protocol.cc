#include "protocol.hh"
#include "application/control.hh"

Protocol::Protocol(class Scene& scene)
    : ansi_(scene), fortuna_(scene), scene_(scene)
{
}

std::vector<uint8_t> Protocol::execute_outputs()
{
    // return events_.get_lastest_events() + fortuna_.get_lastest_fortuna_outputs() + fortuna_.output_collisions();
    return fortuna_.get_lastest_fortuna_outputs();
}

void Protocol::execute_inputs(std::vector<uint8_t> const& data_received)
{
    if (data_received.empty())
        return;

    if (scene_.mode() == Mode::Text) {
        ansi_.process_input(data_received);
    }

#warning TODO - write Protocol::execute_inputs
#if 0
    if (data_received.empty())
        return;

    if (scene_.mode() == Mode::Text) {
        size_t pos = data_received.find("\xfd\x03");
        if (pos == std::string::npos) {
            ansi_.process_input(data_received);
        } else {
            ansi_.process_input(data_received.substr(0, pos));
            fortuna_.process_inputs(data_received.substr(pos + 2));
            control_queue.emplace(ControlCommand::SetMode, Mode::Graphics);
        }

    } else {
        fortuna_.process_inputs(data_received);
    }
#endif
}

void Protocol::reset()
{
    ansi_.reset_protocol();
    fortuna_.reset_protocol();
}

void Protocol::reset_mode()
{
    ansi_.process_input({ ' ', '\b' });
}

void Protocol::event_text_input(std::string const &text)
{
    if (scene_.mode() == Mode::Text)
        ansi_.event_text_input(text);
    else
        fortuna_.event_text_input(text);
}

void Protocol::event_key(uint8_t key, bool is_down, KeyMod mod)
{
    if (scene_.mode() == Mode::Text)
        ansi_.event_key(key, is_down, mod);
    else
        fortuna_.event_key(key, is_down, mod);
}

void Protocol::event_key(SpecialKey key, bool is_down, KeyMod mod)
{
    if (scene_.mode() == Mode::Text)
        ansi_.event_key(key, is_down, mod);
    else
        fortuna_.event_key(key, is_down, mod);
}

void Protocol::event_mouse_button(int button, int x, int y, bool is_down)
{
    if (scene_.mode() == Mode::Graphics)
        fortuna_.event_mouse_button(button, x, y, is_down);
}

void Protocol::event_mouse_move(int button, int x, int y)
{
    if (scene_.mode() == Mode::Graphics)
        fortuna_.event_mouse_move(button, x, y);
}

void Protocol::event_joystick(size_t joystick_number, size_t button, bool is_down)
{
    if (scene_.mode() == Mode::Graphics)
        fortuna_.event_joystick(joystick_number, button, is_down);
}

void Protocol::event_joystick_directional(size_t joystick_number, int8_t axis, int8_t value)
{
    if (scene_.mode() == Mode::Graphics)
        fortuna_.event_joystick_directional(joystick_number, axis, value);
}
