#ifndef PROTOCOL_HH_
#define PROTOCOL_HH_

#include <cstdint>
#include <memory>

#include "../options.hh"
#include "../scene/scene.hh"
#include "../event/outputevent.hh"
#include "event/inputevent.hh"

class Protocol {
public:
    virtual ~Protocol() = default;

    static std::unique_ptr<Protocol> make_protocol(Options const &options, InputQueue &input_queue, OutputQueue &output_queue);

    virtual void input(uint8_t byte) = 0;

protected:
    Protocol(InputQueue& input_queue, OutputQueue &output_queue) : input_queue_(input_queue), output_queue_(output_queue) {}

    InputQueue& input_queue_;
    OutputQueue &output_queue_;
};

#endif //PROTOCOL_HH_
