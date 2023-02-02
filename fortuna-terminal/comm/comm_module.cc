#include "comm_module.hh"

#include <iostream>
#include <cstring>

#include "echo.hh"
#include "tcpip.hh"
#include "debug.hh"

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
        case CommunicationMode::TcpIp:
            return std::make_unique<TCPIP>(output_queue, input_queue, protocol, options.tcpip);
        case CommunicationMode::Debug:
            return std::make_unique<Debug>(output_queue, input_queue, protocol);
        default:
            std::cerr << "Unsupported communication module.\n";
            exit(EXIT_FAILURE);
    }
}

void CommunicationModule::notify_vsync()
{
    // TODO
}

void CommunicationModule::error_message(std::string const &msg, bool describe_errno)
{
    std::string pmsg = msg;
    if (describe_errno)
        pmsg += std::string(": ") + strerror(errno) + "\n";
    else
        pmsg += ".\n";

    std::cerr << pmsg;

    input_queue_.enqueue({ InputEventType::TextSetColor, Color::RED });
    for (char c: pmsg)
        input_queue_.enqueue({ InputEventType::TextPrintChar, (uint8_t) c });
    input_queue_.enqueue(InputEvent { InputEventType::TextResetFormatting });
}