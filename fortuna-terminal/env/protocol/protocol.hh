#ifndef PROTOCOL_HH_
#define PROTOCOL_HH_

#include <string>

#include "ansi.hh"
#include "common/enums/mode.hh"
#include "common/types/time.hh"
#include "fortuna.hh"
#include "events.hh"

class Protocol {
public:
    explicit Protocol(class Scene& scene);

    void                 execute_inputs(std::string const& data_received);
    std::vector<uint8_t> execute_outputs();

    void reset();
    void reset_mode();

    Events& events() { return events_; }

private:
    ANSI            ansi_;
    FortunaProtocol fortuna_;
    Events          events_;
    Scene const&    scene_;
};

#endif //PROTOCOL_HH_
