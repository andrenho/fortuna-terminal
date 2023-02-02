#include "options.hh"

#include <iostream>

#include <getopt.h>

static void print_help(int exit_status)
{
    std::cout <<R"(    -c, --communication-mode        One of "echo", "uart", "i2c", "spi", "tcpip", "debug"
    -t, --terminal-type             One of "sdl", "text" (default: sdl)
    -r, --protocol                  One of "fortuna", "ansi" or "fortuna+ansi" (default: ansi)
    -w, --window                    Window mode (as opposed to the default, which is full screen)
    -d, --debug                     Print lots of debugging information
Options valid for `uart`:
    -P, --serial-port               Serial port (default: /dev/serial0)
    -B, --baud                      Baud speed for UART (default: 57600)
    -U, --uart-settings             Data bits, parity, stop bits (default: 8N1)
Options valid for `tcpip`:
    -R, --tcpip-port                TCP/IP port (default: 8076)
)";
    exit(exit_status);
}

Options::Options(int argc, char **argv)
{
    int c;

    while (true) {
        int option_index = 0;
        static struct option long_options[] = {
                { "communication-mode", required_argument, nullptr, 'c' },
                { "terminal-type", required_argument, nullptr, 't' },
                { "protocol", required_argument, nullptr, 'r' },
                { "window", no_argument, nullptr, 'w' },
                { "debug", no_argument, nullptr, 'd' },
                // serial
                { "serial-port", required_argument, nullptr, 'P' },
                { "baud", required_argument, nullptr, 'B' },
                { "uart-settings", required_argument, nullptr, 'U' },
                // tcp/ip
                { "tcpip-port", required_argument, nullptr, 'R' },
                // end
                { nullptr, 0, nullptr, 0 },
        };

        c = getopt_long(argc, argv, "c:t:r:hdwP:B:U:R:", long_options, &option_index);
        if (c == -1)
            break;

        switch (c) {
            case 0:
                std::cout << "option " << long_options[option_index].name;
                if (optarg)
                    std::cout << " with arg " << optarg;
                std::cout << "\n";
                break;

            case 'h':
                print_help(EXIT_SUCCESS);
                break;

            case 'c': {
                std::string comm(optarg);
                if (comm == "echo")
                    communication_mode = CommunicationMode::Echo;
                else if (comm == "uart")
                    communication_mode = CommunicationMode::UART;
                else if (comm == "i2c")
                    communication_mode = CommunicationMode::I2C;
                else if (comm == "spi")
                    communication_mode = CommunicationMode::SPI;
                else if (comm == "tcpip")
                    communication_mode = CommunicationMode::TcpIp;
                else if (comm == "debug")
                    communication_mode = CommunicationMode::Debug;
                else {
                    std::cerr << "Unsupported communication mode.\n";
                    exit(EXIT_FAILURE);
                }
                break;
            }

            case 't': {
                std::string tt(optarg);
                if (tt == "sdl")
                    terminal_type = TerminalType::SDL;
                else if (tt == "text")
                    terminal_type = TerminalType::Text;
                else {
                    std::cerr << "Unsupported terminal type.\n";
                    exit(EXIT_FAILURE);
                }
                break;
            }

            case 'r': {
                std::string pp(optarg);
                if (pp == "fortuna")
                    protocol = ProtocolType::Fortuna;
                else if (pp == "ansi")
                    protocol = ProtocolType::Ansi;
                else if (pp == "fortuna+ansi")
                    protocol = ProtocolType::Fortuna_Ansi;
                else {
                    std::cerr << "Unsupported protocol.\n";
                    exit(EXIT_FAILURE);
                }
                break;
            }

            case 'd':
                debug_mode = true;
                break;

            case 'w':
                window_mode = true;
                break;

            case 'B':
                try {
                    serial.baud = std::stoi(optarg);
                } catch (std::exception&) {
                    std::cerr << "Invalid baud number.\n";
                    exit(EXIT_FAILURE);
                }
                break;

            case 'P':
                try {
                    serial.port = optarg;
                } catch (std::exception&) {
                    std::cerr << "Invalid port.\n";
                    exit(EXIT_FAILURE);
                }
                break;

            case 'U':
                parse_uart_settings(optarg);
                break;

            case 'R':
                try {
                    tcpip.port = optarg;
                } catch (std::exception&) {
                    std::cerr << "Invalid TCP/IP port.\n";
                    exit(EXIT_FAILURE);
                }
                break;

            case '?':
                std::cerr << "Invalid switch.\n";
                exit(EXIT_FAILURE);
        }

        if (optind > argc)
            print_help(EXIT_FAILURE);
    }

    validate_options();
}

void Options::validate_options() const
{
    if (communication_mode == CommunicationMode::NotChosen) {
        std::cerr << "Communication mode not chosen.\n";
        exit(EXIT_FAILURE);
    }
}

void Options::parse_uart_settings(std::string const &s)
{
    if (s.length() != 3) {
        std::cerr << "Invalid UART settings.\n";
        exit(EXIT_FAILURE);
    }
    if (s[0] != '8') {
        std::cerr << "Invalid UART settings: only 8-bit communication is supported.\n";
        exit(EXIT_FAILURE);
    }
    serial.stop_bits = s[1] - '0';
    serial.parity = s[2];
}
