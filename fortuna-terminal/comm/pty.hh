#ifndef PTY_HH_
#define PTY_HH_

#include "comm_module.hh"
#include "../event/outputevent.hh"
#include "../event/inputevent.hh"
#include "../protocol/protocol.hh"

class PTY : public CommunicationModule {
public:
    void initialize() override;
    void finalize() override;

    void run_input_from_device_thread() override;
    void run_output_to_device_thread() override;

private:
    int master_fd = 0;

    void client_disconnected();
};

#endif //PTY_HH_
