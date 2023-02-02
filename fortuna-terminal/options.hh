#ifndef OPTIONS_HH_
#define OPTIONS_HH_

#include <string>

enum class CommunicationMode {
    NotChosen, Echo, UART, I2C, SPI, TcpIp, Emcc, Debug, PTY
};

enum class TerminalType {
    SDL, Text,
};

enum class ProtocolType {
    Fortuna, Ansi, Fortuna_Ansi,
};

struct SerialOptions {
    size_t            baud = 56700;
    std::string       port = "/dev/serial0";
    char              parity = 'N';
    uint8_t           stop_bits = 1;
};

struct TCPIPOptions {
    std::string port = "8076";
};

struct PTYOptions {
    std::string shell = "/bin/sh";
};

class Options {
public:
    void              parse_commandline_args(int argc, char* argv[]);

    CommunicationMode communication_mode = CommunicationMode::NotChosen;
    TerminalType      terminal_type = TerminalType::SDL;
    ProtocolType      protocol = ProtocolType::Fortuna;
    bool              debug_mode = false;
    bool              debug_bytes = false;
    bool              window_mode = false;
    SerialOptions     serial;
    TCPIPOptions      tcpip;
    PTYOptions        pty;

private:
    void validate_options() const;
    void parse_uart_settings(std::string const& s);
};

#endif
