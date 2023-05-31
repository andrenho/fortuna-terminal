#ifndef ANSI_HH_
#define ANSI_HH_

#include "common/types/noncopyable.hh"

#include <functional>
#include <optional>
#include <string>
#include <vector>
#include <unordered_map>

#include "scene/scene.hh"
#include "scene/ievent.hh"

extern "C" {
#include "lib/tmt/tmt.h"
}

class ANSI : public NonCopyable {
    using Cache = std::vector<TMTCHAR>;

public:
    explicit ANSI(Scene& scene);

    void     process_input(std::vector<uint8_t> const& bytes);

    void     reset_protocol();

    void     event_text_input(std::string const& text);
    void     event_key(uint8_t key, bool is_down, KeyMod mod);
    void     event_key(SpecialKey key, bool is_down, KeyMod mod);

    std::vector<uint8_t> output();

private:
    Scene& scene_;
    Mode current_mode_ = Mode::Text;
    std::vector<uint8_t> output_;

    Cache cache_ {};
    std::unique_ptr<TMT, std::function<void(TMT*)>> vt_ = nullptr;

    void                resize_text_terminal(Mode mode);

    static Char::Attrib translate_attrib(TMTATTRS tmtattrs);
    static void         tmt_callback(tmt_msg_t m, TMT *vt, void const *a, void *p);
    static bool         tmtchar_not_equals(TMTCHAR const &c1, TMTCHAR const &c2);
    static std::optional<std::string> translate_special_key(SpecialKey special_key, KeyMod mod);

    void                initialize_cache();
};

#endif //ANSI_HH_
