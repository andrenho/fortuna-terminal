#ifndef OPTIONS_HH_
#define OPTIONS_HH_

#include <string>

enum class CommunicationMode {
    NotChosen, Echo, UART, I2C, SPI, TcpIp, Emcc
};

enum class TerminalType {
    SDL, Text,
};

enum class ProtocolType {
    Fortuna, Ansi, Fortuna_Ansi,
};

struct SerialOptions {
    unsigned int      baud = 56700;
    std::string       port = "/dev/serial0";
    char              parity = 'N';
    uint8_t           stop_bits = 1;
};

class Options {
public:
    Options(int argc, char* argv[]);

    CommunicationMode communication_mode = CommunicationMode::NotChosen;
    TerminalType      terminal_type = TerminalType::SDL;
    ProtocolType      protocol = ProtocolType::Fortuna;
    bool              debug_mode = false;
    bool              window_mode = false;
    SerialOptions     serial;

private:
    void validate_options() const;
    void parse_uart_settings(std::string const& s);
};

#endif
