#ifndef PROTOCOL_HH_
#define PROTOCOL_HH_

#include "ansi.hh"
#include "extra.hh"
#include "common/mode.hh"
#include "scene/scene.hh"
#include "common/time.hh"
#include "events.hh"

class Protocol : ANSI, Extra, public Events {
public:
    explicit Protocol(Mode initial_mode, Scene& scene, SyncQueueByte& output_queue);

    void execute_inputs(SyncQueueByte &input_queue);
    void execute_outputs(SyncQueueByte &output_queue);

    void reset();

private:
    Scene& scene_;

    void output_pending_responses(SyncQueueByte &output_queue);

    void output_collisions(SyncQueueByte &output_queue);
};

#endif //PROTOCOL_HH_
