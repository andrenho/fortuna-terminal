#include "fortuna.hh"
#include "application/debug.hh"
#include "varint.hh"
#include "application/control.hh"
#include "fortuna_commands.hh"

void FortunaProtocol::process_inputs(std::vector<uint8_t> const &bytes)
{
    size_t pos = 0;

    /*
    auto get_parameters = [&](size_t n_params) -> std::vector<int> {
        auto [pcount, pars] = from_varint(bytes, pos, 4);
        if (pcount == 0)
            return {};  // TODO - use exception
        pos += pcount;
        return pars;
    };
     */

    while (pos < bytes.size()) {

        auto [count, command] = from_varint(bytes, pos, 1);
        if (count == 0)
            return;

        ++pos;

        switch (static_cast<InputCommand>(command.at(0))) {

            case I_RESET_TERMINAL:
                control_queue.emplace(ControlCommand::ResetProtocol);
                debug().info("fortuna: reset protocol");
                break;

            case I_CHANGE_PALETTE: {
                auto [pcount, pars] = from_varint(bytes, pos, 4);
                if (pcount == 0)
                    return;
                pos += pcount;
                scene_.palette[pars[0]] = { (uint8_t) pars[1], (uint8_t) pars[2], (uint8_t) pars[3] };
                debug().info("fortuna: palette set");
                break;
            }

            default:
                fprintf(stderr, "fortuna: invalid command '%d'", command.at(0));
                break;
        }

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