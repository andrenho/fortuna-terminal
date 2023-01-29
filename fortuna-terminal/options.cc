#include "options.hh"

#include <iostream>

#include <getopt.h>

static void print_help(int exit_status)
{
    std::cout << R"(
    -c, --communication-mode        One of "echo", "uart-a", "uart-b", "uart-c", "i2c", "spi", "tcp-ip", "emcc"
    -t, --terminal-type             One of "sdl", "text" (default: sdl)
    -b, --baud                      Baud speed for UART (default: 57600)
    -p, --port                      TCP/IP port (default: 8027)
    )";
    exit(exit_status);
}

Options::Options(int argc, char **argv)
{
    int c;

    while (true) {
        int this_option_optind = optind ? optind : 1;
        int option_index = 0;
        static struct option long_options[] = {
                { 0, 0, 0, 0 },
        };

        c = getopt_long(argc, argv, "c:t:b:h", long_options, &option_index);
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
                    communicationMode = CommunicationMode::Echo;
                else if (comm == "uart-a")
                    communicationMode = CommunicationMode::UART_A;
                else if (comm == "uart-b")
                    communicationMode = CommunicationMode::UART_B;
                else if (comm == "uart-c")
                    communicationMode = CommunicationMode::UART_C;
                else if (comm == "i2c")
                    communicationMode = CommunicationMode::I2C;
                else if (comm == "spi")
                    communicationMode = CommunicationMode::SPI;
                else if (comm == "tcp-ip")
                    communicationMode = CommunicationMode::TcpIp;
                else if (comm == "emcc")
                    communicationMode = CommunicationMode::Emcc;
                else {
                    std::cerr << "Unsupported communication mode.\n";
                    exit(EXIT_FAILURE);
                }
                break;
            }

            case 't': {
                std::string tt(optarg);
                if (tt == "sdl")
                    terminalType = TerminalType::SDL;
                else if (tt == "text")
                    terminalType = TerminalType::Text;
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
    if (communicationMode == CommunicationMode::NotChosen) {
        std::cerr << "Communication mode not chosen.\n";
        exit(EXIT_FAILURE);
    }
}
