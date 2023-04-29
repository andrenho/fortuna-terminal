#include "fortuna.hh"
#include "application/debug.hh"
#include "varint.hh"

void FortunaProtocol::send_bytes(std::string const &bytes)
{
    size_t pos = 0;
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

            if (collision.type == Collision::Colliding)
                str = (char) C_COLLISION_EVT + to_varint({ (int) collision.sprite_a, (int) collision.sprite_b });
            else
                str = (char) C_SEPARATION_EVT + to_varint({ (int) collision.sprite_a, (int) collision.sprite_b });

            debug().debug("%s: sprite %zu and sprite %zu.", collision.type == Collision::Colliding ? "Collision" : "Separation",
                          collision.sprite_a, collision.sprite_b);
        }
    }
    return str;
}