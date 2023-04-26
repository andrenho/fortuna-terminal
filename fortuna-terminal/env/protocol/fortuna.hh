#ifndef FORTUNA_HH_
#define FORTUNA_HH_

#include <sstream>
#include <string>
#include <vector>

#include "scene/scene.hh"
#include "application/gpio.hh"

class FortunaProtocol {
public:
    explicit FortunaProtocol(Scene& scene) : scene_(scene) {}

    std::string send_bytes(std::string const &bytes);

    void reset_protocol();

    std::string get_lastest_fortuna_outputs();
    std::string output_collisions();

private:
    Scene& scene_;

    std::string current_escape_sequence_ {};

    void execute_escape_sequence();
    char parse_escape_sequence(std::vector<ssize_t>& parameters) const;

    std::stringstream fortuna_output_queue_;
};

#endif //FORTUNA_HH_
