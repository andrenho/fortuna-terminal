#ifndef APPLICATION_HH_
#define APPLICATION_HH_

#include <vector>

#include "common/types/noncopyable.hh"
#include "common/enums/execution.hh"
#include "env/environment.hh"
#include "timingdebug.hh"
#include "options.hh"
#include "terminal/terminal.hh"

class Application : NonCopyable {
public:
    Application(int argc, char* argv[]);

    void run();

private:
    Options                  options_;
    Terminal                 terminal_;
    TimingDebug              timing_debug_;
    Gpio                     gpio_ {};
    std::vector<Environment> environments {};
    int                      current_env_idx;
    TimePoint                frame_start_;

    ExecutionStatus execute_single_step();
    ExecutionStatus on_error(std::exception const& exception);

    void            wait_until_end_of_frame();
    void            execute_control_queue();

    static constexpr size_t FPS = 30;
};

#endif //APPLICATION_HH_
