#ifndef APPLICATION_HH_
#define APPLICATION_HH_

#include <vector>

#include "common/noncopyable.hh"
#include "options.hh"
#include "environment/environment.hh"

class Application : NonCopyable {
public:
    Application(int argc, char* argv[]);

    void run();

private:
    Options                  options;
    std::vector<Environment> envs {};
};

#endif //APPLICATION_HH_
