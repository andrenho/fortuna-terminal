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
    ansi_.process_input(" \b");
}
