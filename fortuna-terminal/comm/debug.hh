#ifndef DEBUGMODE_HH_
#define DEBUGMODE_HH_

#include "comm_module.hh"

class Debug : public CommunicationModule {
public:
    void run_input_from_device_thread() override;
    void run_output_to_device_thread() override;
};

#endif //DEBUGMODE_HH_
