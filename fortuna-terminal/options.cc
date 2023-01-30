#include "options.hh"

#include <iostream>

#include <getopt.h>

static void print_help(int exit_status)
{
    std::cout << R"(-c, --communication-mode        One of "echo", "uart-a", "uart-b", "uart-c", "i2c", "spi", "tcp-ip", "emcc"
    -t, --terminal-type             One of "sdl", "text" (default: sdl)
    -b, --baud                      Baud speed for UART (default: 57600)
    -p, --port                      TCP/IP port (default: 8027)
    -d, --debug                     Print lots of debugging information
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
                { "baud", required_argument, nullptr, 'b' },
                { "port", required_argument, nullptr, 'p' },
                { "debug", no_argument, nullptr, 'd' },
                { nullptr, 0, nullptr, 0 },
        };

        c = getopt_long(argc, argv, "c:t:b:hd", long_options, &option_index);
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
                else if (comm == "uart-a")
                    communication_mode = CommunicationMode::UART_A;
                else if (comm == "uart-b")
                    communication_mode = CommunicationMode::UART_B;
                else if (comm == "uart-c")
                    communication_mode = CommunicationMode::UART_C;
                else if (comm == "i2c")
                    communication_mode = CommunicationMode::I2C;
                else if (comm == "spi")
                    communication_mode = CommunicationMode::SPI;
                else if (comm == "tcp-ip")
                    communication_mode = CommunicationMode::TcpIp;
                else if (comm == "emcc")
                    communication_mode = CommunicationMode::Emcc;
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

            case 'b':
                try {
                    baud = std::stoi(optarg);
                } catch (std::exception&) {
                    std::cerr << "Invalid baud number.\n";
                    exit(EXIT_FAILURE);
                }
                break;

            case 'p':
                try {
                    port = std::stoi(optarg);
                } catch (std::exception&) {
                    std::cerr << "Invalid port number.\n";
                    exit(EXIT_FAILURE);
                }
                break;

            case 'd':
                debug_mode = true;
                break;

            case '?':
                exit(EXIT_FAILURE);
        }

        if (optind < argc)
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
