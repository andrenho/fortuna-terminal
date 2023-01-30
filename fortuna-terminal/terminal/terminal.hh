#ifndef TERMINAL_HH_
#define TERMINAL_HH_

#include <memory>
#include "../options.hh"
#include "../comm/comm_module.hh"

class Terminal
{
protected:
    explicit Terminal(CommunicationModule& comm) : comm_(comm) {}

public:
    virtual ~Terminal() = default;

    virtual void initialize() {}
    virtual void update() = 0;

    bool running() const { return running_; }

    static std::unique_ptr<Terminal> make_terminal(const Options &options, CommunicationModule& comm);

protected:
    CommunicationModule& comm_;
    bool                 running_ = true;
};

#endif //TERMINAL_HH_
