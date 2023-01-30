#ifndef OPTIONS_HH_
#define OPTIONS_HH_

enum class CommunicationMode {
    NotChosen, Echo, UART, I2C, SPI, TcpIp, Emcc
};

enum class TerminalType {
    SDL, Text,
};

enum class ProtocolType {
    Fortuna, Ansi, Fortuna_Ansi,
};

class Options {
public:
    Options(int argc, char* argv[]);

    CommunicationMode communication_mode = CommunicationMode::NotChosen;
    TerminalType      terminal_type = TerminalType::SDL;
    ProtocolType      protocol = ProtocolType::Fortuna;
    unsigned int      baud = 56700;
    unsigned int      port = 8027;
    bool              debug_mode = false;
    bool              window_mode = false;

private:
    void validate_options() const;
};

#endif
