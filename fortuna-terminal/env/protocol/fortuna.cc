#include "fortuna.hh"
#include "application/debug.hh"
#include "varint.hh"
#include "application/control.hh"

void FortunaProtocol::process_inputs(std::vector<uint8_t> const &bytes)
{
    size_t pos = 0;
    auto [count, command] = from_varint(bytes, 1);
    if (count == 0)
        return;

    switch (command.at(0)) {
        case 0:
            control_queue.emplace(ControlCommand::ResetProtocol);
            debug().info("fortuna: reset protocol");
            break;
        default:
            fprintf(stderr, "fortuna: invalid command '%d'", command.at(0));
            break;
    }
}

void FortunaProtocol::reset_protocol()
{
    current_str_.clear();
}

std::string FortunaProtocol::get_lastest_fortuna_outputs()
{
    std::string events = fortuna_output_queue_.str();
    fortuna_output_queue_.str("");
    return events;
}

std::string FortunaProtocol::output_collisions()
{
    std::string str;
    if (scene_.mode() == Mode::Graphics) {
        for (auto const& collision: scene_.sprites().check_for_new_collisions()) {

            /*  TODO
            if (collision.type == Collision::Colliding)
                str = (char) C_COLLISION_EVT + to_varint({ (int) collision.sprite_a, (int) collision.sprite_b });
            else
                str = (char) C_SEPARATION_EVT + to_varint({ (int) collision.sprite_a, (int) collision.sprite_b });
                */

            debug().debug("%s: sprite %zu and sprite %zu.", collision.type == Collision::Colliding ? "Collision" : "Separation",
                          collision.sprite_a, collision.sprite_b);
        }
    }
    return str;
}