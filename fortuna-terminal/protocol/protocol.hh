#ifndef PROTOCOL_HH_
#define PROTOCOL_HH_

#include <memory>
#include "../options.hh"
#include "../scene/scene.hh"
#include "../event/outputevent.hh"

class Protocol {
public:
    static std::unique_ptr<Protocol> make_protocol(const Options &options, OutputQueue& queue, Scene& scene);

protected:
    Protocol(OutputQueue& queue, Scene& scene) : queue_(queue), scene_(scene) {}

    OutputQueue& queue_;
    Scene& scene_;
};

#endif //PROTOCOL_HH_
