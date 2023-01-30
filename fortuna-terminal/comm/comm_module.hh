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
    virtual void run([[maybe_unused]] Protocol const* protocol) {}
    virtual void finalize() { running_ = false; };

    virtual void notify_vsync();

    static std::unique_ptr<CommunicationModule> make_communication_module(Options const& options);

protected:
    bool running_ = true;
};

#endif //COMM_MODULE_HH_
