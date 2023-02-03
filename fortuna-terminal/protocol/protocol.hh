#ifndef PROTOCOL_HH_
#define PROTOCOL_HH_

#include <cstdint>
#include <memory>
#include <vector>

#include "../options.hh"
#include "../scene/scene.hh"
#include "../event/outputevent.hh"
#include "event/inputevent.hh"

#include "../../fortuna-protocol.h"

class Protocol {
public:
    virtual ~Protocol() = default;

    virtual void input_char(uint8_t byte) = 0;

    virtual void output_key_event(bool is_down, uint8_t key_code, KeyMod keymod) = 0;
    virtual void output_special_key_event(bool is_down, SpecialKey special_key, KeyMod keymod) = 0;

    static std::unique_ptr<Protocol> make_protocol();

protected:
    void debug_special(std::string const &str) const;
    void debug_special(std::string const &str, uint8_t p1) const;
    void debug_special(std::string const &str, uint8_t p1, uint8_t p2) const;
};

#endif //PROTOCOL_HH_
