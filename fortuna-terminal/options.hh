#ifndef OPTIONS_HH_
#define OPTIONS_HH_

enum class CommunicationMode {
    NotChosen, Echo, UART_A, UART_B, UART_C, I2C, SPI, TcpIp, Emcc
};

enum class TerminalType {
    SDL, Text,
};

class Options {
public:
    Options(int argc, char* argv[]);

    CommunicationMode communicationMode = CommunicationMode::NotChosen;
    TerminalType terminalType = TerminalType::SDL;
    unsigned int baud = 56700;
    unsigned int port = 8027;

private:
    void validate_options() const;
};

#endif
