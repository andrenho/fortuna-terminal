#ifndef OPTIONS_HH_
#define OPTIONS_HH_

#include <string>

enum class CommType { NotChosen, Uart, I2C, SPI, TcpIp, Pipes, PTY, Echo };

struct TerminalOptions {
    bool window_mode = false;
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

struct Options {
public:
    Options(int argc, char* argv[]);

    CommType        comm_type   = CommType::NotChosen;
    bool            debug_comm = false;
    bool            graphics_mode = false;

    TerminalOptions terminal_options {};
    UartOptions     uart_options {};
    TcpIpOptions    tcpip_options {};
    PTYOptions      pty_options {};

private:
    [[ noreturn ]] void print_help(int exit_status);
    void parse_uart_settings(std::string const& opt);
};

#endif //OPTIONS_HH_
