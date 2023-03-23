#ifndef ENVIRONMENT_HH_
#define ENVIRONMENT_HH_

#include "application/options.hh"
#include "common/syncqueue.hh"
#include "env/comm/comm.hh"
#include "env/protocol/protocol.hh"
#include "scene/scene.hh"
#include "common/iterativethread.hh"
#include "application/framecontrol.hh"

class Environment {
public:
    explicit Environment(Options const& options);

    void execute_single_step(FrameControl& frame_control);
    void reset();

    void show_error(std::exception const &e);

    void set_mode(Mode mode);

    [[nodiscard]] Scene const& scene() const { return scene_; }
    IEvent&                    events_interface() { return protocol_; }

private:
    CommUniquePtr                  comm_;
    Scene                          scene_;
    Protocol                       protocol_;
    bool                           show_fps_counter_;

    [[nodiscard]] std::string welcome_message() const;
    void display_debugging_info(FrameControl const &frame_control);
};

#endif //ENVIRONMENT_HH_
