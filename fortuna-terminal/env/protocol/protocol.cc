#include "protocol.hh"
#include "application/control.hh"

Protocol::Protocol(class Scene& scene)
    : ansi_(scene), fortuna_(scene), scene_(scene)
{
}

std::string Protocol::execute_outputs()
{
    return events_.get_lastest_events() + fortuna_.get_lastest_fortuna_outputs() + fortuna_.output_collisions();
}

void Protocol::execute_inputs(std::string const& data_received)
{
    if (data_received.empty())
        return;

    if (scene_.mode() == Mode::Text) {
        size_t pos = data_received.find("\xfd\x03");
        if (pos == std::string::npos) {
            ansi_.send_bytes(data_received);
        } else {
            ansi_.send_bytes(data_received.substr(0, pos));
            fortuna_.send_bytes(data_received.substr(pos + 2));
            control_queue.emplace(ControlCommand::SetMode, Mode::Graphics);
        }

    } else {
        fortuna_.send_bytes(data_received);
    }
}

void Protocol::reset()
{
    ansi_.reset_protocol();
    fortuna_.reset_protocol();
}

void Protocol::reset_mode()
{
    ansi_.send_bytes(" \b");
}
