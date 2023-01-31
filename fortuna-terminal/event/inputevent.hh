#ifndef INPUTEVENT_HH_
#define INPUTEVENT_HH_

#include <cstdint>
#include "../util/sync_queue.hh"

enum class InputEventType {
    NoEvent, TextPrintChar, TextMoveUp, TextMoveDown, TextMoveForward, TextMoveBackward, TextMoveTo, TextSetColor,
    TextClearScreen, TextResetFormatting,
};

struct InputEvent {
    InputEventType type = InputEventType::NoEvent;
    union {
        struct {
            uint8_t p1 = 0;
            uint8_t p2 = 0;
        };
    };

    InputEvent() {}
    explicit InputEvent(InputEventType type) : type(type) {}
    InputEvent(InputEventType type, uint8_t p1) : type(type), p1(p1) {}
    InputEvent(InputEventType type, uint8_t p1, uint8_t p2) : type(type), p1(p1), p2(p2) {}
};

using InputQueue = SyncQueue<InputEvent>;

#endif //INPUTEVENT_HH_
