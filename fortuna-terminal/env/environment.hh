#ifndef ENVIRONMENT_HH_
#define ENVIRONMENT_HH_

#include "application/options.hh"
#include "common/syncqueue.hh"
#include "comm/comm.hh"
#include "env/protocol/protocol.hh"
#include "scene/scene.hh"
#include "common/iterativethread.hh"
#include "application/timingdebug.hh"

class Environment {
public:
    explicit Environment(Options const& options);

    void execute_single_step(TimingDebug& timing_debug);
    void reset();

    void show_error(std::exception const &e) const;

    void set_mode(Mode mode);

    [[nodiscard]] Scene const& scene() const { return scene_; }
    IEvent&                    events_interface() { return protocol_; }

private:
    CommUniquePtr                  comm_;
    Scene                          scene_;
    Protocol                       protocol_;
    bool                           show_timining_info_;
    bool                           debug_comm_;

    [[nodiscard]] std::string welcome_message() const;
    void display_timing_info(TimingDebug const &timing_debug) const;
    void debug_bytes(std::string_view received, std::string_view sent) const;
};

#endif //ENVIRONMENT_HH_
