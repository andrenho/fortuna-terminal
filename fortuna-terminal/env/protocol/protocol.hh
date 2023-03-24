#ifndef PROTOCOL_HH_
#define PROTOCOL_HH_

#include <string>

#include "ansi.hh"
#include "common/enums/mode.hh"
#include "common/types/time.hh"
#include "fortuna.hh"
#include "events.hh"

class Protocol :
        private ANSI,
        private FortunaProtocol,
        public Events {
public:
    Protocol(class Scene& scene);

    void        execute_inputs(std::string const& data_received);
    std::string execute_outputs();

    void reset();
    void reset_mode();

private:
    bool        extra_active_ = false;
    std::string received_bytes_;
};

#endif //PROTOCOL_HH_
