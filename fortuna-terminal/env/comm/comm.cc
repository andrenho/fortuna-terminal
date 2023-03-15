#include "comm.hh"

#include "common/exceptions/fortunaexception.hh"
#include "env/comm/io/fd/tcpip.hh"
#include "env/comm/io/fd/pipes.hh"
#if INCLUDE_UART_PTY
#  include "env/comm/io/fd/uart.hh"
#  include "env/comm/io/fd/pty.hh"
#endif
#include "env/comm/xchg/echoxchg.hh"
#include "io/echo.hh"

std::unique_ptr<CommunicationModule> CommunicationModule::create(Options const &options)
{
    switch (options.comm_type) {
        case CommType::TcpIp:
            return std::make_unique<TCPIP>(options.tcpip_options);
        case CommType::Echo:
            return std::make_unique<Echo>();
        case CommType::EchoXchg:
            return std::make_unique<EchoXchg>();
        case CommType::Pipes:
            return std::make_unique<Pipes>();
#if INCLUDE_UART_PTY
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
        case CommType::I2C:
        case CommType::SPI:
            throw FortunaException("Communication mode not yet implemented");
        default:
            throw FortunaException("Unsupported communication module");
    }
}
