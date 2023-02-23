#ifndef ANSI_HH_
#define ANSI_HH_

#include "../common/noncopyable.hh"

#include <optional>
#include <string>

#include "terminal/scene/scene.hh"
#include "keys.hh"

extern "C" {
#include "lib/tmt/tmt.h"
}

class ANSI : public NonCopyable {
public:
    explicit ANSI(Scene& scene);

    void                       send_bytes(std::vector<uint8_t> const& bytes);
    std::optional<std::string> translate_special_key(SpecialKey special_key, KeyMod mod);

    void reset();

private:
    Scene& scene_;
    std::unordered_map<uint8_t, std::unordered_map<uint8_t, TMTCHAR>> cache_;
    std::unique_ptr<TMT, std::function<void(TMT*)>> vt_ = nullptr;

    static CharAttrib translate_attrib(TMTATTRS tmtattrs);
    static void tmt_callback(tmt_msg_t m, TMT *vt, void const *a, void *p);
    static std::unordered_map<uint8_t, std::unordered_map<uint8_t, TMTCHAR>> initialize_cache(Size initial_size);
};

#endif //ANSI_HH_
