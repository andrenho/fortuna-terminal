#ifndef ANSIPROTOCOL_HH_
#define ANSIPROTOCOL_HH_

#include <functional>
#include <memory>
#include <thread>
#include <vector>

#include "comm/comm.hh"
#include "terminal/scene/sceneevent.hh"
#include "common/syncqueue.hh"
#include "common/geometry.hh"

extern "C" {
#include "lib/tmt/tmt.h"
}

class AnsiProtocol /* : public NonCopyable */ {
public:
    AnsiProtocol(std::unique_ptr<CommunicationModule> comm, SyncQueue<SceneEvent>& scene_queue, unsigned int scene_n, Size const& size);

    void run();
    void do_events(SyncQueue<FP_Message> &event_queue);

    void finalize_threads();

private:
    static void tmt_callback(tmt_msg_t m, TMT *vt, const void *a, void *p);

    std::unique_ptr<CommunicationModule> comm_;
    SyncQueue<SceneEvent>& scene_queue_;
    unsigned int scene_n_;

    std::unique_ptr<std::thread> read_thread_ = nullptr;
    std::unique_ptr<std::thread> input_thread_ = nullptr;
    bool                         threads_active_ = true;

    std::unique_ptr<SyncQueue<uint8_t>> input_queue_ = std::make_unique<SyncQueue<uint8_t>>();
    std::unordered_map<uint8_t, std::unordered_map<uint8_t, TMTCHAR>> cache_;

    std::unique_ptr<TMT, std::function<void(TMT*)>> vt_ = nullptr;

    static CharAttrib translate_attrib(TMTATTRS tmtattrs);

    static std::unordered_map<uint8_t, std::unordered_map<uint8_t, TMTCHAR>> initialize_cache(Size size);
};

#endif //ANSIPROTOCOL_HH_
