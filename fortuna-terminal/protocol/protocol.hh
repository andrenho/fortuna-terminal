#ifndef PROTOCOL_HH_
#define PROTOCOL_HH_

#include <cstdint>
#include <memory>
#include <vector>

#include "../options.hh"
#include "../scene/scene.hh"
#include "../event/outputevent.hh"
#include "event/inputevent.hh"

struct KeyModifiers {
    bool shift   = false;
    bool control = false;
    bool alt     = false;
};

enum struct SpecialKey : uint8_t {
    Esc, F1, F2, F3, F4, F5, F6, F7, F8, F9, F10, F11, F12, Tab, CapsLock, Win,
    Insert, Home, End, PageUp, PageDown, Up, Down, Left, Right, Enter, Backspace,
    Delete, PrintScreen, PauseBreak,
};

class Protocol {
public:
    explicit Protocol(OutputQueue& output_queue) : output_queue_(output_queue) {}
    virtual ~Protocol() = default;

    virtual void input(uint8_t byte, InputQueue& input_queue) = 0;

    virtual void output_key_event(bool is_down, uint8_t key_code, KeyModifiers key_modifiers) = 0;
    virtual void output_special_key_event(bool is_down, SpecialKey special_key, KeyModifiers key_modifiers) = 0;

    static std::unique_ptr<Protocol> make_protocol(Options const &options, OutputQueue& output_queue);

protected:
    OutputQueue& output_queue_;
};

#endif //PROTOCOL_HH_
