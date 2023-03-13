#ifndef ANSI_HH_
#define ANSI_HH_

#include "common/types/noncopyable.hh"

#include <functional>
#include <optional>
#include <string>
#include <unordered_map>

#include "scene/scene.hh"
#include "keys.hh"

extern "C" {
#include "lib/tmt/tmt.h"
}

class ANSI : public NonCopyable {
    using Cache = std::vector<TMTCHAR>;

public:
    explicit ANSI(Mode initial_mode, Scene& scene);

    void     send_ansi_bytes(std::string const& bytes);

    void     reset_ansi_protocol();

private:
    Scene& scene_;
    Mode current_mode_;

    Cache cache_ {};
    std::unique_ptr<TMT, std::function<void(TMT*)>> vt_ = nullptr;

    void              resize_text_terminal(Mode mode);

    static Cache      initialize_cache(size_t w, size_t h);
    static CharAttrib translate_attrib(TMTATTRS tmtattrs);
    static void       tmt_callback(tmt_msg_t m, TMT *vt, void const *a, void *p);
    static bool       tmtchar_not_equals(TMTCHAR const &c1, TMTCHAR const &c2);
};

#endif //ANSI_HH_
