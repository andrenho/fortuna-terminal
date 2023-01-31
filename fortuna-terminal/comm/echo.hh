#ifndef ECHO_HH_
#define ECHO_HH_

#include "comm_module.hh"

class Echo : public CommunicationModule {
public:
    Echo(OutputQueue& output_queue, InputQueue& input_queue, Protocol& protocol)
        : CommunicationModule(output_queue, input_queue, protocol) {}

    void run_input_from_device_thread() override;
    void run_output_to_device_thread() override;

    void finalize() override;

private:
    SyncQueue<uint8_t> buffer;
};

#endif //ECHO_HH_
