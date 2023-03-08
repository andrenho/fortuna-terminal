#ifndef APPLICATION_HH_
#define APPLICATION_HH_

#include <vector>

#include "common/noncopyable.hh"
#include "options.hh"
#include "env/environment.hh"
#include "framecontrol.hh"
#include "terminal/terminal.hh"

class Application : NonCopyable {
public:
    Application(int argc, char* argv[]);

    void run();

private:
    Options                  options_;
    Terminal                 terminal_;
    GPIO                     gpio_ {};
    std::vector<Environment> envs {};
    int                      current_env = -1;
    FrameControl             frame_control_ {};

    void execute_control_commands();

    void on_error(std::exception const &e, bool& quit);
};

#endif //APPLICATION_HH_
