#ifndef ENVIRONMENT_HH_
#define ENVIRONMENT_HH_

#include "application/options.hh"
#include "env/comm/comm.hh"
#include "common/syncqueue.hh"
#include "env/protocol/protocol.hh"
#include "env/scene/scene.hh"

class Environment {
public:
    explicit Environment(Options const& options);

private:
    CommUniquePtr             comm_;
    SyncQueueUniqPtr<uint8_t> input_queue_ {};
    SyncQueueUniqPtr<uint8_t> output_queue_ {};
    Protocol                  protocol_ {};
};

#endif //ENVIRONMENT_HH_
