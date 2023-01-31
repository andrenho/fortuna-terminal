#ifndef OUTPUTEVENT_HH_
#define OUTPUTEVENT_HH_

#include <cstdint>

#include "../util/sync_queue.hh"

enum class OutputEventType { Keydown, Keyup };
enum struct KeyType : uint8_t { Regular, Special };
enum struct SpecialKey : uint8_t {
    Esc, F1, F2, F3, F4, F5, F6, F7, F8, F9, F10, F11, F12, Tab, CapsLock, Win,
    Insert, Home, End, PageUp, PageDown, Up, Down, Left, Right,
};

struct KeyCode {
    union {
        uint8_t    code;
        SpecialKey special_key;
    };
    KeyType key_type : 1;
    bool    shift    : 1;
    bool    control  : 1;
    bool    alt      : 1;
};

struct OutputEvent {
    OutputEventType type;
    union {
        KeyCode key_code;
    };
};

using OutputQueue = SyncQueue<OutputEvent>;

#endif //OUTPUTEVENT_HH_
