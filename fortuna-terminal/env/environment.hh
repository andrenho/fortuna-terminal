#ifndef ENVIRONMENT_HH_
#define ENVIRONMENT_HH_

#include "application/options.hh"
#include "common/syncqueue.hh"
#include "env/comm/comm.hh"
#include "env/protocol/protocol.hh"
#include "scene/scene.hh"
#include "common/iterativethread.hh"

class Environment {
public:
    explicit Environment(Options const& options);

    void execute_single_step(size_t avg_fps);
    void reset();

    void run_threads(bool debug_comm);
    void finalize_threads();

    void show_error(std::exception const &e);

    void set_mode(Mode mode);

    [[nodiscard]] Scene const& scene() const { return scene_; }
    IEvent&                    events_interface() { return protocol_; }

    SyncQueueByte& input_queue() { return *input_queue_; }
    std::string    communication_module_description() const { return comm_->description(); }

private:
    std::unique_ptr<SyncQueueByte> input_queue_ = std::make_unique<SyncQueueByte>();
    std::unique_ptr<SyncQueueByte> output_queue_ = std::make_unique<SyncQueueByte>();

    CommUniquePtr                 comm_;
    Scene                         scene_;
    Protocol                      protocol_;
    bool                          show_fps_counter_;

    void show_fps_counter(size_t fps);
};

#endif //ENVIRONMENT_HH_
