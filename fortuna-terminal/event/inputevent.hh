#ifndef INPUTEVENT_HH_
#define INPUTEVENT_HH_

#include <cstdint>
#include "../util/sync_queue.hh"

enum class InputEventType {
    TextPrintChar, TextMoveUp, TextMoveDown, TextMoveForward, TextMoveBackward, TextMoveTo, TextSetColor,
    TextClearScreen, TextResetFormatting,
};

struct InputEvent {
    InputEventType type;
    union {
        uint8_t p1;
    };
};

using InputQueue = SyncQueue<InputEvent>;

#endif //INPUTEVENT_HH_
