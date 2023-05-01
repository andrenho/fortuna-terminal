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

    void process_inputs(std::vector<uint8_t> const &bytes);

    void reset_protocol();

    std::string get_lastest_fortuna_outputs();
    std::string output_collisions();

private:
    Scene& scene_;

    std::string current_str_ {};

    std::stringstream fortuna_output_queue_;
};

#endif //FORTUNA_HH_
