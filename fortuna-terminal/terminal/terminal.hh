#ifndef TERMINAL_HH_
#define TERMINAL_HH_

#include <memory>
#include "../options.hh"
#include "../comm/comm_module.hh"

class Terminal
{
public:
    virtual ~Terminal() = default;

    virtual void initialize() {}
    virtual void do_events() = 0;
    virtual void draw() const = 0;

    [[nodiscard]] bool running() const { return running_; }

    static std::unique_ptr<Terminal> make_terminal();

protected:
    bool         running_ = true;
};

#endif //TERMINAL_HH_
