#ifndef COMM_MODULE_HH_
#define COMM_MODULE_HH_

#include <memory>
#include <vector>

#include "../util/sync_queue.hh"
#include "event/inputevent.hh"
#include "event/outputevent.hh"
#include "protocol/protocol.hh"

class CommunicationModule
{
public:
    virtual ~CommunicationModule() = default;

    virtual void initialize() {}
    virtual void run_input_from_device_thread(Protocol*, InputQueue*) {}
    virtual void run_output_to_device_thread(OutputQueue*) {}
    virtual void finalize() { running_ = false; };

    virtual void notify_vsync();

    static std::unique_ptr<CommunicationModule> make_communication_module(class Options const& options);

protected:
    bool running_ = true;
};

#endif //COMM_MODULE_HH_
