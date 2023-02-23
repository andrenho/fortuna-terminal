#ifndef EXTRA_HH_
#define EXTRA_HH_

#include <string>
#include <vector>

#include "../terminal/scene/scene.hh"
#include "../gpio/gpio.hh"

class Terminal;

class Extra {
public:
    explicit Extra(Terminal& terminal, Scene& scene, GPIO& gpio) : terminal_(terminal), scene_(scene), gpio_(gpio) {}

    void send_bytes(std::vector<uint8_t> const &bytes);

    bool escape_sequence_active() const;

private:
    Terminal& terminal_;
    Scene& scene_;
    GPIO& gpio_;

    std::string escape_sequence_;

    void escape_sequence_complete();
    char parse_escape_sequence(std::vector<size_t>& parameters) const;
};

#endif //EXTRA_HH_
