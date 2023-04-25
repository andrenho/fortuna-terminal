#include "protocol.hh"

Protocol::Protocol(class Scene& scene)
    : ansi_(scene)
{
}

std::string Protocol::execute_outputs()
{
    return events_.get_lastest_events() /* + get_lastest_fortuna_outputs() + output_collisions() */;
}

void Protocol::execute_inputs(std::string const& data_received)
{
    if (data_received.empty())
        return;

    ansi_.send_bytes(data_received);
}

void Protocol::reset()
{
    ansi_.reset_protocol();
    // reset_fortuna_protocol();
}

void Protocol::reset_mode()
{
    ansi_.send_bytes(" \b");
}
