#ifndef ECHO_HH_
#define ECHO_HH_

#include "comm_module.hh"

class Echo : public CommunicationModule {
public:
    void run_input_from_device_thread() override;
    void run_output_to_device_thread() override;

    void finalize() override;

private:
    SyncQueue<uint8_t> buffer;
};

#endif //ECHO_HH_
