#ifndef ENV_HH_
#define ENV_HH_

#include "../options.hh"
#include "../common/syncqueue.hh"

class Env {
public:
    Env(ProtocolType protocol_type, CommType comm_type, SyncQueue<struct FP_Message>& scene_queue);

    void run();
    void event(struct FP_Message&& message);
};

#endif //ENV_HH_
