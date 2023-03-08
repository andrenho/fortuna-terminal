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

    Scene const &scene() const { return scene_; }

    Events &events_interface() { return protocol_; }

    void reset();

    void set_mode(Mode mode);

private:
    std::unique_ptr<SyncQueueByte> input_queue_ = std::make_unique<SyncQueueByte>();
    std::unique_ptr<SyncQueueByte> output_queue_ = std::make_unique<SyncQueueByte>();

    void show_fps_counter(Duration duration);

    CommUniquePtr comm_;
    Scene         scene_;
    Protocol      protocol_;
    bool          show_fps_counter_;
};

#endif //ENVIRONMENT_HH_
