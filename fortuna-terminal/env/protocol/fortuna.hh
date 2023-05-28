#ifndef FORTUNA_HH_
#define FORTUNA_HH_

#include <span>
#include <sstream>
#include <string>
#include <vector>

#include "scene/scene.hh"
#include "application/gpio.hh"

class FortunaProtocol {
public:
    explicit FortunaProtocol(Scene& scene);

    void process_inputs(std::span<const uint8_t> const& bytes);
    void process_inputs(std::vector<uint8_t> const& bytes);

    void reset_protocol();

    std::vector<uint8_t> get_lastest_fortuna_outputs();
    std::string output_collisions();

private:
    Scene&               scene_;
    std::vector<uint8_t> current_input_;
    std::vector<int>     fortuna_output_queue_;

    size_t process_input_vector(std::span<const uint8_t> const &bytes);
    uint8_t checksum(std::vector<uint8_t> const& bytes) const;

    static constexpr uint8_t FRAME_END[] = { 0x54, 0xee, 0xc2, 0x28 };
};

#endif //FORTUNA_HH_
