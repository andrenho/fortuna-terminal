#include "protocol.hh"

using namespace std::string_literals;

Protocol::Protocol(Mode initial_mode, Scene& scene, SyncQueueByte& output_queue)
    : ANSI(initial_mode, scene),
      Extra(scene),
      Events(output_queue),
      scene_(scene)
{
}

void Protocol::execute_outputs(SyncQueueByte& output_queue)
{
    output_pending_responses(output_queue);
    output_collisions(output_queue);
}

void Protocol::output_collisions(SyncQueueByte &output_queue)
{
    if (scene_.mode() == Mode::Graphics) {
        for (auto const& collision: scene_.sprites().check_for_new_collisions()) {
            output_queue.push_all("\e#"s +
                                  (collision.type == Collision::Colliding ? "1" : "0") +
                                  ";" + std::to_string(collision.sprite_a) + ";" + std::to_string(collision.sprite_b));
        }
    }
}

void Protocol::output_pending_responses(SyncQueueByte &output_queue)
{
    std::string response = latest_response();
    if (!response.empty())
        output_queue.push_all(response);
}

void Protocol::execute_inputs(SyncQueueByte& input_queue)
{
    static std::string received_bytes;
    static bool extra_active = false;

    input_queue.optionally_pop_all_into(received_bytes);

    bool last_is_esc = received_bytes.ends_with('\e');
    if (last_is_esc)
        received_bytes = received_bytes.substr(0, received_bytes.size() - 1);

    if (!received_bytes.empty()) {

        size_t current_pos = 0;
        while (true) {
            if (!extra_active) {
                // we're not in extra escape sequence, so let's look for a "ESC *" that starts that sequence
                // if we don't find it, we can send the whole thing to ANSI
                size_t next_esc_star = received_bytes.find("\e*", current_pos);
                if (next_esc_star == std::string::npos) {
                    send_ansi_bytes(received_bytes.substr(current_pos));
                    break;
                } else {
                    extra_active = true;
                    send_ansi_bytes(received_bytes.substr(current_pos, next_esc_star - current_pos));
                    current_pos = next_esc_star;
                }
            } else {
                auto it_next_alpha = std::find_if(received_bytes.begin() + current_pos, received_bytes.end(), [](char c) { return std::isalpha(c); });
                if (it_next_alpha == received_bytes.end()) {
                    send_extra_bytes(received_bytes.substr(current_pos));
                    break;
                } else {
                    size_t offset = it_next_alpha - received_bytes.begin() + 1;
                    extra_active = false;
                    send_extra_bytes(received_bytes.substr(current_pos, offset - current_pos));
                    current_pos = offset;
                }
            }
        }

        received_bytes.clear();
    }
    if (last_is_esc)
        received_bytes.push_back('\e');
}

void Protocol::reset()
{
    // TODO
}
