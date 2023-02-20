#ifndef OPTIONS_HH_
#define OPTIONS_HH_

#include <string>

enum class ProtocolType { Ansi, Fortuna };
enum class CommType { Uart, I2C, SPI, TcpIp, Pipes, PTY, Echo, Debug };

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

class Options {

};

#endif //OPTIONS_HH_
