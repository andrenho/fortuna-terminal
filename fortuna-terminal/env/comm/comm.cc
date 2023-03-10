#include "comm.hh"
#include "io/echo.hh"
#include "application/options.hh"
#include "common/exceptions/fortunaexception.hh"
#include "application/options.hh"
#include "env/comm/io/fd/tcpip.hh"
#include "env/comm/io/fd/pipes.hh"
#include "env/comm/io/fd/uart.hh"
#include "env/comm/io/fd/pty.hh"

std::unique_ptr<CommunicationModule> CommunicationModule::create(Options const &options)
{
    switch (options.comm_type) {
        case CommType::NotChosen:
            throw FortunaException("Invalid communication mode");
        case CommType::I2C:
        case CommType::SPI:
            throw FortunaException("Communication mode not yet implemented");
#ifndef _WIN32
        case CommType::Uart:
            return std::make_unique<UART>(options.uart_options);
        case CommType::PTY:
            return std::make_unique<PTY>(options.pty_options);
            break;
#else
        case CommType::Uart:
        case CommType::PTY:
            throw FortunaException("Communication mode not supported on Windows");
#endif
        case CommType::TcpIp:
            return std::make_unique<TCPIP>(options.tcpip_options);
        case CommType::Echo:
            return std::make_unique<Echo>();
        case CommType::Pipes:
            return std::make_unique<Pipes>();
    }

    // TODO - add other communcation modules

    return nullptr;
}
