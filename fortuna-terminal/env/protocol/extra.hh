#ifndef EXTRA_HH_
#define EXTRA_HH_

#include <string>
#include <vector>

#include "env/scene/scene.hh"
#include "old/gpio/gpio.hh"

class Extra {
public:
    explicit Extra(Scene& scene) : scene_(scene) {}

    void send_extra_bytes(std::string const &bytes);

    std::string latest_response();

private:
    Scene& scene_;

    std::string escape_sequence_ {};

    std::string response_;

    void escape_sequence_complete();
    char parse_escape_sequence(std::vector<ssize_t>& parameters) const;
};

#endif //EXTRA_HH_
