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

    void send_bytes(std::string const &bytes);

    void reset_protocol();

    std::string get_lastest_fortuna_outputs();
    std::string output_collisions();

private:
    Scene& scene_;

    std::string current_str_ {};

    std::stringstream fortuna_output_queue_;

    enum Event : uint8_t {
        C_COLLISION_EVT  = 0x11,
        C_SEPARATION_EVT = 0x12,
    };

    static std::string to_fortuna(uint8_t cmd, std::vector<int> const& pars);
    static std::pair<bool, int> from_fortuna(std::string_view str);
};

#endif //FORTUNA_HH_
