#ifndef APPLICATION_HH_
#define APPLICATION_HH_

#include "common/noncopyable.hh"

class Application : NonCopyable {
public:
    Application(int argc, char* argv[]);

    void run();
};

#endif //APPLICATION_HH_
