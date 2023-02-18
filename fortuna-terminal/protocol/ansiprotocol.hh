#ifndef ANSIPROTOCOL_HH_
#define ANSIPROTOCOL_HH_

#include <functional>
#include <memory>
#include <vector>

#include "protocol.hh"
#include "common/geometry.hh"

extern "C" {
#include "lib/tmt/tmt.h"
}

class AnsiProtocol : public Protocol {
public:
    AnsiProtocol(std::unique_ptr<CommunicationModule> comm, SyncQueue<SceneEvent> &scene_queue, unsigned int scene_n, Size size);
    ~AnsiProtocol() override;

    void run() override;
    void do_events(SyncQueue<FP_Message> &event_queue) override;

private:
    static void tmt_callback(tmt_msg_t m, TMT *vt, const void *a, void *p);

    std::unique_ptr<std::thread> read_thread_ = nullptr;
    std::unique_ptr<std::thread> input_thread_ = nullptr;
    bool                         threads_active_ = true;

    std::unique_ptr<TMT, std::function<void(TMT*)>> vt_ = nullptr;

    static CharAttrib translate_attrib(TMTATTRS tmtattrs);
};

#endif //ANSIPROTOCOL_HH_
