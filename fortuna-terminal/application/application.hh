#ifndef APPLICATION_HH_
#define APPLICATION_HH_

#include <vector>

#include "common/noncopyable.hh"
#include "common/execution.hh"
#include "env/environment.hh"
#include "framecontrol.hh"
#include "options.hh"
#include "terminal/terminal.hh"

class Application : NonCopyable {
public:
    Application(int argc, char* argv[]);

    void run();

private:
    Options                  options_;
    Terminal                 terminal_;
    GPIO                     gpio_ {};
    std::vector<Environment> environments {};
    int                      current_env_idx;
    FrameControl             frame_control_ {};

    ExecutionStatus execute_single_step();
    ExecutionStatus on_error(std::exception const& exception);

    void            execute_control_queue();
};

#endif //APPLICATION_HH_
