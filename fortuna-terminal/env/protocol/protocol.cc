#include "protocol.hh"

Protocol::Protocol(Mode initial_mode, class Scene& scene, SyncQueueByte& output_queue)
    : ANSI(initial_mode, scene),
      FortunaProtocol(scene, output_queue),
      Events(output_queue)
{
}

void Protocol::execute_outputs()
{
    output_collisions();
}

void Protocol::execute_inputs(SyncQueueByte& input_queue)
{
    input_queue.optionally_pop_all_into(received_bytes_);

    bool last_is_esc = received_bytes_.ends_with('\e');
    if (last_is_esc)
        received_bytes_ = received_bytes_.substr(0, received_bytes_.size() - 1);

    if (!received_bytes_.empty()) {

        size_t current_pos = 0;
        while (true) {
            if (!extra_active_) {
                // we're not in extra escape sequence, so let's look for a "ESC *" that starts that sequence
                // if we don't find it, we can send the whole thing to ANSI
                size_t next_esc_star = received_bytes_.find("\e*", current_pos);
                if (next_esc_star == std::string::npos) {
                    send_ansi_bytes(received_bytes_.substr(current_pos));
                    break;
                } else {
                    extra_active_ = true;
                    send_ansi_bytes(received_bytes_.substr(current_pos, next_esc_star - current_pos));
                    current_pos = next_esc_star;
                }
            } else {
                auto it_next_alpha = std::find_if(received_bytes_.begin() + current_pos, received_bytes_.end(), [](char c) { return std::isalpha(c); });
                if (it_next_alpha == received_bytes_.end()) {
                    send_fortuna_bytes(received_bytes_.substr(current_pos));
                    break;
                } else {
                    size_t offset = it_next_alpha - received_bytes_.begin() + 1;
                    extra_active_ = false;
                    send_fortuna_bytes(received_bytes_.substr(current_pos, offset - current_pos));
                    current_pos = offset;
                }
            }
        }

        received_bytes_.clear();
    }
    if (last_is_esc)
        received_bytes_.push_back('\e');
}

void Protocol::reset()
{
    reset_ansi_protocol();
    reset_fortuna_protocol();
}
