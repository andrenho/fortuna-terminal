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
    explicit Protocol(Mode initial_mode, class Scene& scene, SyncQueueByte& output_queue);

    void execute_inputs(SyncQueueByte &input_queue);
    void execute_outputs();

    void reset();

private:
    bool        extra_active_ = false;
    std::string received_bytes_;
};

#endif //PROTOCOL_HH_
