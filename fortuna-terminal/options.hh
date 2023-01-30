#ifndef OPTIONS_HH_
#define OPTIONS_HH_

extern bool debug_mode;

enum class CommunicationMode {
    NotChosen, Echo, UART_A, UART_B, UART_C, I2C, SPI, TcpIp, Emcc
};

enum class TerminalType {
    SDL, Text,
};

class Options {
public:
    Options(int argc, char* argv[]);

    CommunicationMode communication_mode = CommunicationMode::NotChosen;
    TerminalType      terminal_type = TerminalType::SDL;
    unsigned int      baud = 56700;
    unsigned int      port = 8027;
    bool              debug_mode = false;

private:
    void validate_options() const;
};

#endif
