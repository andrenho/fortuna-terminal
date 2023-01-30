#ifndef OUTPUT_EVENT_HH_
#define OUTPUT_EVENT_HH_

#include <cstdint>

enum class OutputEventType { Keydown };
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

#endif //OUTPUT_EVENT_HH_
