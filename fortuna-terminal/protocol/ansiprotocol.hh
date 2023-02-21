#ifndef ANSIPROTOCOL_HH_
#define ANSIPROTOCOL_HH_

#include <functional>
#include <memory>
#include <thread>
#include <vector>

#include "comm/comm.hh"
#include "terminal/scene/scene.hh"
#include "common/syncqueue.hh"
#include "common/geometry.hh"

extern "C" {
#include "lib/tmt/tmt.h"
}

enum class SpecialKey {
    ESC, F1, F2, F3, F4, F5, F6, F7, F8, F9, F10, F11, F12, TAB, CAPSLOCK, WIN,
    INSERT, HOME, END, PAGEUP, PAGEDOWN, UP, DOWN, LEFT, RIGHT, ENTER, BACKSPACE,
    DELETE, PRINTSCREEN, PAUSEBREAK,
};

struct KeyMod {
    bool shift;
    bool control;
    bool alt;
};

class AnsiProtocol /* : public NonCopyable */ {
public:
    explicit AnsiProtocol(std::unique_ptr<CommunicationModule> comm);

    void run();

    void finalize_threads();

    void event_text_input(std::string const& text);
    void event_key(uint8_t key, bool is_down, KeyMod mod);
    void event_key(SpecialKey key, bool is_down, KeyMod mod);

    Scene const& scene() const { return scene_; }
    Scene& scene() { return scene_; }

private:
    static void tmt_callback(tmt_msg_t m, TMT *vt, const void *a, void *p);

    std::unique_ptr<CommunicationModule> comm_;
    Scene scene_;

    std::unique_ptr<std::thread> read_thread_ = nullptr;
    std::unique_ptr<std::thread> input_thread_ = nullptr;
    std::unique_ptr<std::thread> output_thread_ = nullptr;
    bool                         threads_active_ = true;

    std::unique_ptr<SyncQueue<uint8_t>> input_queue_ = std::make_unique<SyncQueue<uint8_t>>();
    std::unique_ptr<SyncQueue<uint8_t>> output_queue_ = std::make_unique<SyncQueue<uint8_t>>();
    std::unordered_map<uint8_t, std::unordered_map<uint8_t, TMTCHAR>> cache_;

    std::unique_ptr<TMT, std::function<void(TMT*)>> vt_ = nullptr;

    static CharAttrib translate_attrib(TMTATTRS tmtattrs);

    static std::unordered_map<uint8_t, std::unordered_map<uint8_t, TMTCHAR>> initialize_cache(Size size);
};

#endif //ANSIPROTOCOL_HH_
