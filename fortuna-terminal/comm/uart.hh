#ifndef UART_HH_
#define UART_HH_

#include "comm_module.hh"

class UART : public CommunicationModule {
public:
    explicit UART(OutputQueue& output_queue, InputQueue& input_queue, SerialOptions const& serial_options, Protocol& protocol)
        : CommunicationModule(output_queue, input_queue, protocol), serial_options_(serial_options) {}

    void initialize() override;

    void run_input_from_device_thread() override;
    void run_output_to_device_thread() override;

    void finalize() override;

private:
    void error_message(std::string const& msg, bool describe_errno);

    SerialOptions const& serial_options_;
    int fd = 0;
};

#endif //UART_HH_
