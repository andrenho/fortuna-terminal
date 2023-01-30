#ifndef COMM_MODULE_HH_
#define COMM_MODULE_HH_

#include <memory>
#include "../options.hh"
#include "../event/output_event.hh"
#include "../util/sync_queue.hh"

class CommunicationModule
{
protected:
    explicit CommunicationModule(SyncQueue<OutputEvent>& output_event_queue) : output_event_queue(output_event_queue) {}
    bool running_ = true;
    SyncQueue<OutputEvent>& output_event_queue;

public:
    virtual void initialize() {}
    virtual void run() {}
    virtual void finalize() { running_ = false; };

    static std::unique_ptr<CommunicationModule> make_communication_module(Options const& options, SyncQueue<OutputEvent>& output_event_queue);
};

#endif //COMM_MODULE_HH_
