#include "comm.hh"
#include "options.hh"
#include "exceptions/fortunaexception.hh"
#include "echo.hh"
#include "tcpip.hh"
#ifndef _WIN32
#  include "pty.hh"
#  include "uart.hh"
#endif

std::optional<uint8_t> CommunicationModule::read_blocking()
{
    auto v = read_blocking(1);
    if (v.empty())
        return {};
    else
        return v.at(0);
}

std::unique_ptr<CommunicationModule> CommunicationModule::create_unique(Options const *options, Size const &size)
{
    switch (options->comm_type) {
        case CommType::NotChosen:
            throw FortunaException("Invalid communication mode");
        case CommType::I2C:
        case CommType::SPI:
        case CommType::Pipes:
        case CommType::Debug:
            throw FortunaException("Communication mode not yet implemented");
#ifndef _WIN32
        case CommType::Uart:
            return std::make_unique<UART>()
        case CommType::PTY:
            return std::make_unique<PTY>(options->pty_options, size);
            break;
#else
        case CommType::Uart:
        case CommType::PTY:
            throw FortunaException("Communication mode not supported on Windows");
#endif
        case CommType::TcpIp:
            return std::make_unique<TCPIP>(options->tcpip_options);
        case CommType::Echo:
            return std::make_unique<Echo>();
    }
    return nullptr;
}
