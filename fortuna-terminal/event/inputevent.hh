#ifndef INPUTEVENT_HH_
#define INPUTEVENT_HH_

#include <cstdint>
#include "../util/sync_queue.hh"

enum class InputEventType { PrintChar };

struct InputEvent {
    InputEventType type;
    union {
        uint8_t print_char;
    };
};

using InputQueue = SyncQueue<InputEvent>;

#endif //INPUTEVENT_HH_
