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
    virtual void do_events(OutputQueue& output_queue) = 0;
    virtual void draw(Scene const& scene) const = 0;

    bool running() const { return running_; }

    static std::unique_ptr<Terminal> make_terminal(const Options &options);

protected:
    bool         running_ = true;
};

#endif //TERMINAL_HH_
