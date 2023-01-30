#ifndef COMM_MODULE_HH_
#define COMM_MODULE_HH_

#include <memory>
#include "../options.hh"
#include "../util/sync_queue.hh"
#include "../protocol/protocol.hh"

class CommunicationModule
{
public:
    virtual void initialize() {}
    virtual void run() {}
    virtual void finalize() { running_ = false; };

    static std::unique_ptr<CommunicationModule> make_communication_module(Options const& options, Protocol const* protocol);

protected:
    explicit CommunicationModule(Protocol const* protocol) : protocol_(protocol) {}

    bool running_ = true;
    Protocol const* protocol_;
};

#endif //COMM_MODULE_HH_
