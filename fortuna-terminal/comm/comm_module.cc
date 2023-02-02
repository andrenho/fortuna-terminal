#include "comm_module.hh"

#include <iostream>
#include <cstring>
#include <iomanip>

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
            return std::make_unique<UART>();
#endif
#ifdef COMM_PTY
        case CommunicationMode::PTY:
            return std::make_unique<PTY>();
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

void CommunicationModule::debug_received_byte(uint8_t byte) const
{
    if (options.debug_bytes) {
        if (byte >= 32 && byte < 127)
            std::cout << "[\e[0;32m" << (char) byte << "\e[0m]" << std::flush;
        else
            std::cout << "[\e[0;32m" << std::hex << std::setw(2) << std::setfill('0') << byte << " \e[0m]" << std::flush;
    }
}

void CommunicationModule::debug_sent_byte(uint8_t byte) const
{
    if (options.debug_bytes) {
        if (byte >= 32 && byte < 127)
            std::cout << "'\e[0;31m" << (char) byte << "\e[0m' " << std::flush;
        else
            std::cout << "\e[0;31m" << std::hex << std::setw(2) << std::setfill('0') << byte << " \e[0m" << std::flush;
    }
}
