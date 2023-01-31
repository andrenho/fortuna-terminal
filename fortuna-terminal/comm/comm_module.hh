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
    CommunicationModule(OutputQueue& output_queue, InputQueue& input_queue, Protocol& protocol)
        : output_queue_(output_queue), input_queue_(input_queue), protocol_(protocol) {}
    virtual ~CommunicationModule() = default;

    virtual void initialize() {}
    virtual void run_input_from_device_thread() {}
    virtual void run_output_to_device_thread() {}
    virtual void finalize() { running_ = false; };

    virtual void notify_vsync();

    static std::unique_ptr<CommunicationModule> make_communication_module(
            Options const& options, OutputQueue& output_queue, InputQueue& input_queue, Protocol& protocol);

protected:
    bool running_ = true;
    OutputQueue& output_queue_;
    InputQueue& input_queue_;
    Protocol& protocol_;
};

#endif //COMM_MODULE_HH_
