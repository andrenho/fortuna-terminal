#ifndef PROTOCOL_HH_
#define PROTOCOL_HH_

#include <cstdint>
#include <memory>
#include <vector>

#include "../options.hh"
#include "../scene/scene.hh"
#include "../event/outputevent.hh"
#include "event/inputevent.hh"

class Protocol {
public:
    virtual ~Protocol() = default;

    static std::unique_ptr<Protocol> make_protocol(Options const &options);

    virtual void input(uint8_t byte, InputQueue& input_queue) const = 0;
    virtual std::vector<uint8_t> process_output_queue(OutputQueue& output_queue) const = 0;
};

#endif //PROTOCOL_HH_
