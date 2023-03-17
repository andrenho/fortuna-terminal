#include "comm.hh"

#include "common/exceptions/fortunaexception.hh"
#include "env/comm/io/fd/tcpip.hh"
#include "env/comm/io/fd/pipes.hh"
#ifdef COMM_UART
#  include "env/comm/io/fd/uart.hh"
#endif
#ifdef COMM_PTY
#  include "env/comm/io/fd/pty.hh"
#endif
#ifdef COMM_SPI
#  include "env/comm/xchg/spi.hh"
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
        case CommType::Uart:
#ifdef COMM_UART
            return std::make_unique<UART>(options.uart_options);
#else
            throw FortunaException("Communication mode not supported on Windows");
#endif
        case CommType::PTY:
#ifdef COMM_PTY
            return std::make_unique<PTY>(options.pty_options);
#else
            throw FortunaException("Communication mode not supported on Windows");
#endif
        case CommType::SPI:
#ifdef COMM_SPI
            return std::make_unique<SPI>(options.spi_options);
#else
            throw FortunaException("Communication mode not supported except for Raspberry Pi");
#endif
#ifdef COMM_I2C
            return std::make_unique<I2C>(options.i2c_options);
#else
            throw FortunaException("Communication mode not supported except for Raspberry Pi");
#endif
        case CommType::I2C:
        default:
            throw FortunaException("Unsupported communication module");
    }
}
