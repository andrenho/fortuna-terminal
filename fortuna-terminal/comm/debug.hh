#ifndef DEBUGMODE_HH_
#define DEBUGMODE_HH_

#include "comm_module.hh"

class Debug : public CommunicationModule {
public:
    Debug(OutputQueue& output_queue, InputQueue& input_queue, Protocol& protocol)
        : CommunicationModule(output_queue, input_queue, protocol) {}

    void run_input_from_device_thread() override;
    void run_output_to_device_thread() override;
};

#endif //DEBUGMODE_HH_
