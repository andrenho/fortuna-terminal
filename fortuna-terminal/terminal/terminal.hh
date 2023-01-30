#ifndef TERMINAL_HH_
#define TERMINAL_HH_

#include <memory>
#include "../options.hh"
#include "../comm/comm_module.hh"

class Terminal
{
protected:
    explicit Terminal(Scene const& scene, OutputQueue& output_queue) : scene_(scene), output_queue_(output_queue) {}

public:
    virtual ~Terminal() = default;

    virtual void initialize() {}
    virtual void update() = 0;

    bool running() const { return running_; }

    static std::unique_ptr<Terminal> make_terminal(const Options &options, Scene const& scene, OutputQueue& output_queue);

protected:
    Scene const& scene_;
    OutputQueue& output_queue_;
    bool         running_ = true;
};

#endif //TERMINAL_HH_
