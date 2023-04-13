#ifndef OPTIONS_HH_
#define OPTIONS_HH_

#include <string>
#include "common/types/time.hh"
#include "debug.hh"

enum class CommType { NotChosen, Uart, I2C, SPI, TcpIp, Pipes, PTY, Echo, EchoXchg };

struct TerminalOptions {
    bool window_mode = false;
};

struct UartOptions {
    std::string  port      = "/dev/serial0";
    size_t       baud      = 500000;
    char         parity    = 'N';
    uint8_t      stop_bits = 1;
};

struct TcpIpOptions {
    unsigned int port = 8076;
};

struct PTYOptions {
    std::string shell = "/bin/bash";
};

struct SPIOptions {
    size_t   speed = 400000;
    Duration delay = 0ms;
};

struct I2COptions {
    uint8_t address = 0x3b;
};

struct Options {
public:
    Options(int argc, char* argv[]);

    CommType        comm_type   = CommType::NotChosen;
    bool            debug_time = false;
    bool            welcome_message = true;
    size_t          readbuf_sz = 2048;
    DebugVerbosity  debug_verbosity = V_NORMAL;

    TerminalOptions terminal_options {};
    UartOptions     uart_options {};
    TcpIpOptions    tcpip_options {};
    PTYOptions      pty_options {};
    SPIOptions      spi_options {};
    I2COptions      i2c_options {};

private:
    [[ noreturn ]] static void print_help(int exit_status);
    void parse_uart_settings(std::string const& opt);
};

#endif //OPTIONS_HH_
