#ifndef APPLICATION_HH_
#define APPLICATION_HH_

#include "old/common/noncopyable.hh"
#include "old/options.hh"
#include "old/terminal/terminal.hh"
#include "old/comm/comm.hh"
#include "gpio.h"
#include "old/protocol/protocol.hh"

class Application : NonCopyable {
public:
    Application(int argc, char* argv[]);

    void run();

private:
    Options               options;
    Terminal              terminal;
    CommUniquePtr         comm;
    GPIO                  gpio {};
    std::vector<Protocol> protocols;
    Protocol*             current_protocol;

    void on_error(std::exception &e, bool *quit);
};

#endif //APPLICATION_HH_
