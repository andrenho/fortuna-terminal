#ifndef EXTRA_HH_
#define EXTRA_HH_

#include <string>
#include <vector>

#include "scene/scene.hh"
#include "../gpio/gpio.hh"

class Extra {
public:
    explicit Extra(Mode mode, Scene& scene, GPIO& gpio) : mode_(mode), scene_(scene), gpio_(gpio) {}

    void send_bytes(std::string const &bytes);

    void set_mode(Mode mode) { mode_ = mode; }

private:
    Mode mode_;
    Scene& scene_;
    GPIO&  gpio_;

    std::string escape_sequence_ {};

    void escape_sequence_complete();
    char parse_escape_sequence(std::vector<size_t>& parameters) const;
};

#endif //EXTRA_HH_
