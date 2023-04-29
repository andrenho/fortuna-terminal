#include "fortuna.hh"
#include "application/debug.hh"

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
    std::stringstream ss;
    if (scene_.mode() == Mode::Graphics) {
        for (auto const& collision: scene_.sprites().check_for_new_collisions()) {

            if (collision.type == Collision::Colliding)
                ss << to_fortuna(C_COLLISION_EVT, { (int) collision.sprite_a, (int) collision.sprite_b });
            else
                ss << to_fortuna(C_SEPARATION_EVT, { (int) collision.sprite_a, (int) collision.sprite_b });

            debug().debug("%s: sprite %zu and sprite %zu.", collision.type == Collision::Colliding ? "Collision" : "Separation",
                          collision.sprite_a, collision.sprite_b);
        }
    }
    return ss.str();
}

std::string FortunaProtocol::to_fortuna(uint8_t cmd, std::vector<int> const& pars)
{
    std::string str { (char) cmd };

    for (int par: pars) {
        while (par > 127) {
            str += (char) ((par & 0x7f) | 0x80);
            par >>= 7;
        }
        str += (char) (par & 0x7f);
    }

    return str;
}

std::pair<bool, int> FortunaProtocol::from_fortuna(std::string_view str)
{
    int value = 0;
    size_t shift = 0;
    size_t pos = 0;
    size_t len = str.length();

    while (true) {
        if (pos >= len)
            return { false, {} };

        uint8_t byte = str.at(pos++);
        value |= ((int) byte & 0x7f) << shift;
        if (!(byte & 0x80))
            break;
        shift += 7;
    }

    return { true, value };
}
