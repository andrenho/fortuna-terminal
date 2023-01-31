#include "comm_module.hh"

#include <iostream>

#include "echo.hh"
#ifdef COMM_UART
#  include "uart.hh"
#endif

std::unique_ptr<CommunicationModule>
CommunicationModule::make_communication_module(Options const &options, OutputQueue &output_queue,
                                               InputQueue &input_queue, Protocol& protocol)
{
    switch (options.communication_mode) {
        case CommunicationMode::Echo:
            return std::make_unique<Echo>(output_queue, input_queue, protocol);
#ifdef COMM_UART
        case CommunicationMode::UART:
            return std::make_unique<UART>(output_queue, input_queue, protocol, options.serial);
#endif
        default:
            std::cerr << "Unsupported communication module.\n";
            exit(EXIT_FAILURE);
    }
}

void CommunicationModule::notify_vsync()
{
    // TODO
}
