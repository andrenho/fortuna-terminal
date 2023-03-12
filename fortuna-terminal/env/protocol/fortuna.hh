#ifndef FORTUNA_HH_
#define FORTUNA_HH_

#include <string>
#include <vector>

#include "scene/scene.hh"
#include "application/gpio.hh"

class FortunaProtocol {
public:
    FortunaProtocol(Scene& scene, SyncQueueByte& output_queue) : scene_(scene), output_queue_(output_queue) {}

    void send_fortuna_bytes(std::string const &bytes);

    void output_collisions();

    void reset_fortuna_protocol();

private:
    Scene& scene_;
    SyncQueueByte &output_queue_;

    std::string current_escape_sequence_ {};

    void execute_escape_sequence();
    char parse_escape_sequence(std::vector<ssize_t>& parameters) const;
};

#endif //FORTUNA_HH_
