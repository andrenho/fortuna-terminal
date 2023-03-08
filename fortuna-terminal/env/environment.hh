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

    void run_io_threads();
    void execute_step(Duration duration);

    void show_error(std::exception const &e);

    void blink_cursor();

private:
    CommUniquePtr             comm_;
    SyncQueueUniqPtr<uint8_t> input_queue_ {};
    SyncQueueUniqPtr<uint8_t> output_queue_ {};
    Scene                     scene_;
    Protocol                  protocol_;
};

#endif //ENVIRONMENT_HH_
