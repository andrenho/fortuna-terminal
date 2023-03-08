#ifndef ANSI_HH_
#define ANSI_HH_

#include "old/common/noncopyable.hh"

#include <functional>
#include <optional>
#include <string>
#include <unordered_map>

#include "old/scene/scene.hh"
#include "keys.hh"

extern "C" {
#include "lib/tmt/tmt.h"
}

class ANSI : public NonCopyable {
    using Cache = std::vector<TMTCHAR>;

public:
    explicit ANSI(Mode mode, Scene& scene);

    void                       send_ansi_bytes(std::string const& bytes);
    std::optional<std::string> translate_special_key(SpecialKey special_key, KeyMod mod);

    void reset();

    void set_mode(Mode mode);

private:
    Scene& scene_;
    Cache cache_ {};
    std::unique_ptr<TMT, std::function<void(TMT*)>> vt_ = nullptr;

    static CharAttrib translate_attrib(TMTATTRS tmtattrs);
    static void tmt_callback(tmt_msg_t m, TMT *vt, void const *a, void *p);
    static Cache initialize_cache(size_t w, size_t h);

    static bool tmtchar_equals(TMTCHAR const &c1, TMTCHAR const &c2);
};

#endif //ANSI_HH_
