#include "fortuna.hh"
#include "application/debug.hh"
#include "varint.hh"
#include "application/control.hh"
#include "fortuna_commands.hh"

FortunaProtocol::FortunaProtocol(Scene& scene)
    : scene_(scene)
{
    current_input_.reserve(512);
}

void FortunaProtocol::process_inputs(std::span<const uint8_t> const &bytes)
{
    // try to find end-of-frame
    auto frame_end = std::search(bytes.begin(), bytes.end(), std::begin(FRAME_END), std::end(FRAME_END));
    if (frame_end != bytes.end()) {
        // if end-of-frame found, split the input in two and execute them
        process_inputs(std::span<const uint8_t> { bytes.begin(), frame_end });
        current_input_.clear();
        process_inputs(std::span<const uint8_t> { frame_end + 4, bytes.end() });
        return;
    }

    // add received string to the queue
    current_input_.insert(current_input_.end(), bytes.begin(), bytes.end());
    size_t input_size = current_input_.size();

    // process input
    std::span<const uint8_t> data_to_process(current_input_.data(), input_size);
    size_t bytes_processed = process_input_vector(data_to_process);

    // remove processed data from the queue
    current_input_.erase(current_input_.begin(), current_input_.begin() + (ssize_t) bytes_processed);
}

void FortunaProtocol::process_inputs(std::vector<uint8_t> const& bytes)
{
    process_inputs(std::span<const uint8_t>(bytes.begin(), bytes.end()));
}

size_t FortunaProtocol::process_input_vector(std::span<const uint8_t> const &bytes)
{
    size_t pos = 0;

    auto get_parameters = [&](size_t n_params) -> std::vector<int> {
        auto [pcount, pars] = from_varint(bytes.subspan(pos), n_params);
        pos += pcount;
        return pars;
    };

    try {
        while (pos < bytes.size()) {

            // get command

            auto [count, command] = from_varint(bytes.subspan(pos), 1);
            if (count == 0)
                return pos;

            ++pos;

            // parse command

            switch (static_cast<InputCommand>(command.at(0))) {

                case I_RESET_TERMINAL:
                    control_queue.emplace(ControlCommand::ResetProtocol);
                    debug().info("fortuna: reset protocol");
                    break;

                case I_CHANGE_PALETTE: {
                    auto pars = get_parameters(4);
                    scene_.palette[pars[0]] = { (uint8_t) pars[1], (uint8_t) pars[2], (uint8_t) pars[3] };
                    debug().info("fortuna: palette set");
                    break;
                }

                case I_UPLOAD_IMAGE: {
                    auto pars = get_parameters(258);
                    Image image = {
                            .transparent_color = (uint8_t) pars[1],
                            .pixels = {0}
                    };
                    std::transform(pars.begin() + 2, pars.end(), std::begin(image.pixels),
                                   [](int color) { return (uint8_t) color; });
                    scene_.add_image(pars[0], std::move(image));
                    debug().info("fortuna: image index %d created (transparent color is %d)", pars[0], image.transparent_color);
                    break;
                }

                default:
                    debug().warning("fortuna: invalid command '%d'\n", command.at(0));
                    break;
            }

        }

    } catch (VarintInputTooShortException&) {
        return pos - 1;  // stop execution

    }

    return pos;
}

void FortunaProtocol::reset_protocol()
{
    current_input_.clear();
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

uint8_t FortunaProtocol::checksum(std::vector<uint8_t> const& bytes) const
{
    uint8_t checksum = 0;
    for (uint8_t byte: bytes)
        checksum ^= byte;
    return checksum;
}