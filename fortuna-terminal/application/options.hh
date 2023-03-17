#ifndef OPTIONS_HH_
#define OPTIONS_HH_

#include <string>
#include "common/enums/mode.hh"
#include "common/types/time.hh"

enum class CommType { NotChosen, Uart, I2C, SPI, TcpIp, Pipes, PTY, Echo, EchoXchg };

struct TerminalOptions {
    bool window_mode = false;
    bool show_fps_counter = false;
};

struct UartOptions {
    std::string  port      = "/dev/serial0";
    size_t       baud      = 1000000;
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
    size_t   speed = 1000000;
    Duration delay;
};

struct I2COptions {
    uint8_t address = 0x68;
};

struct Options {
public:
    Options(int argc, char* argv[]);

    CommType        comm_type   = CommType::NotChosen;
    bool            debug_comm = false;
    Mode            mode = Mode::Text;

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
