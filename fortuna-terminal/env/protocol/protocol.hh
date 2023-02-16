#ifndef PROTOCOL_HH_
#define PROTOCOL_HH_

#include "../../common/syncqueue.hh"

class Protocol {
protected:
    Protocol(SyncQueue<struct FP_Message>& scene_queue) : scene_queue_(scene_queue) {}

    SyncQueue<struct FP_Message>& scene_queue_;
};

#endif //PROTOCOL_HH_
