#include "comm_module.hh"

#include <iostream>
#include <cstring>

#include "global.hh"

#include "echo.hh"
#include "tcpip.hh"
#include "debug.hh"
#include "pty.hh"

#ifdef COMM_UART
#  include "uart.hh"
#endif

std::unique_ptr<CommunicationModule>
CommunicationModule::make_communication_module()
{
    switch (options.communication_mode) {
        case CommunicationMode::Echo:
            return std::make_unique<Echo>();
        case CommunicationMode::TcpIp:
            return std::make_unique<TCPIP>();
        case CommunicationMode::Debug:
            return std::make_unique<Debug>();
#ifdef COMM_UART
            case CommunicationMode::UART:
            return std::make_unique<UART>(output_queue, input_queue, protocol, options.serial);
#endif
#ifdef COMM_PTY
        case CommunicationMode::PTY:
            return std::make_unique<PTY>(output_queue, input_queue, protocol, options.pty);
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

void CommunicationModule::error_message(std::string const &msg, bool describe_errno)
{
    std::string pmsg = msg;
    if (describe_errno)
        pmsg += std::string(": ") + strerror(errno) + "\n";
    else
        pmsg += ".\n";

    std::cerr << pmsg;

    input_queue.enqueue({ InputEventType::TextSetColor, Color::RED });
    for (char c: pmsg)
        input_queue.enqueue({ InputEventType::TextPrintChar, (uint8_t) c });
    input_queue.enqueue(InputEvent { InputEventType::TextResetFormatting });
}