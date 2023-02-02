#ifndef UART_HH_
#define UART_HH_

#include "comm_module.hh"
#include "../event/outputevent.hh"
#include "../event/inputevent.hh"
#include "../protocol/protocol.hh"

class UART : public CommunicationModule {
public:
    void initialize() override;

    void run_input_from_device_thread() override;
    void run_output_to_device_thread() override;

    void finalize() override;

private:
    int fd = 0;
};

#endif //UART_HH_
