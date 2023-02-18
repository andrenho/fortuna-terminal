#ifndef PROTOCOL_HH_
#define PROTOCOL_HH_

#include <memory>

#include "../common/syncqueue.hh"
#include "../common/noncopyable.hh"
#include "../options.hh"
#include "../terminal/scene/sceneevent.hh"
#include "comm/comm.hh"
#include "common/geometry.hh"

class Protocol : NonCopyable {
public:
    virtual ~Protocol() = default;

    virtual void run() = 0;
    virtual void do_events(SyncQueue<FP_Message>& event_queue) = 0;

    static std::unique_ptr<Protocol>
    create_unique(ProtocolType protocol_type, std::unique_ptr<CommunicationModule> comm,
                  SyncQueue<SceneEvent> &scene_queue,
                  unsigned int scene_n, Size size);

protected:
    Protocol(std::unique_ptr<CommunicationModule> comm, SyncQueue<SceneEvent> &scene_queue, unsigned int scene_n);

    std::unique_ptr<CommunicationModule> comm_;
    SyncQueue<SceneEvent>& scene_queue_;
    unsigned int scene_n_;
};

#endif //PROTOCOL_HH_
