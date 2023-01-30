#ifndef ECHO_HH_
#define ECHO_HH_

#include "comm_module.hh"

class Echo : public CommunicationModule {
public:
    explicit Echo(SyncQueue<OutputEvent>& output_event_queue) : CommunicationModule(output_event_queue) {}
};

#endif //ECHO_HH_
