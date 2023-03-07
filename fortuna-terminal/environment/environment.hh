#ifndef ENVIRONMENT_HH_
#define ENVIRONMENT_HH_

#include "application/options.hh"
#include "environment/comm/comm.hh"
#include "common/syncqueue.hh"
#include "environment/protocol/protocol.hh"
#include "environment/scene/scene.hh"

class Environment {
public:
    explicit Environment(Options const& options);

private:
    CommUniquePtr             comm_ {};
    SyncQueueUniqPtr<uint8_t> input_queue_ {};
    SyncQueueUniqPtr<uint8_t> output_queue_ {};
    Protocol                  protocol_ {};
};

#endif //ENVIRONMENT_HH_
