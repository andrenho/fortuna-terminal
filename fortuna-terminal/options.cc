#include "options.hh"
#include "exceptions/libcexception.hh"
#include "exceptions/fortunaexception.hh"

#include <cstring>
#include <getopt.h>

#include <iostream>

Options::Options(int argc, char* argv[])
{
    int c;

    while (true) {
        int option_index = 0;
        static struct option long_options[] = {
                { "communication-mode", required_argument, nullptr, 'c' },
                { "window",             no_argument,       nullptr, 'w' },
                { "fps",                no_argument,       nullptr, 'f' },
                { "graphics",           no_argument,       nullptr, 'g' },
                { "debug-comm",         no_argument,       nullptr, 'd' },
                // serial
                { "serial-port",        required_argument, nullptr, 'P' },
                { "baud",               required_argument, nullptr, 'B' },
                { "uart-settings",      required_argument, nullptr, 'U' },
                // tcp/ip
                { "tcpip-port",         required_argument, nullptr, 'R' },
                // shell
                { "shell",              required_argument, nullptr, 'S' },
                // end
                { nullptr, 0, nullptr, 0 },
        };

        c = getopt_long(argc, argv, "c:hwP:B:U:R:S:dgf", long_options, &option_index);
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
                if (strcmp(optarg, "echo") == 0)
                    comm_type = CommType::Echo;
                else if (strcmp(optarg, "uart") == 0)
                    comm_type = CommType::Uart;
                else if (strcmp(optarg, "i2c") == 0)
                    comm_type = CommType::I2C;
                else if (strcmp(optarg, "spi") == 0)
                    comm_type = CommType::SPI;
                else if (strcmp(optarg, "tcpip") == 0)
                    comm_type = CommType::TcpIp;
                else if (strcmp(optarg, "pty") == 0)
                    comm_type = CommType::PTY;
                else if (strcmp(optarg, "pipes") == 0)
                    comm_type = CommType::Pipes;
                else
                    throw FortunaException("Unsupported communication mode");
                break;
            }

            case 'w':
                terminal_options.window_mode = true;
                break;

            case 'f':
                terminal_options.show_fps_counter = true;
                break;

            case 'd':
                debug_comm = true;
                break;

            case 'g':
                graphics_mode = true;
                break;

            case 'B':
                uart_options.baud = strtol(optarg, nullptr, 10);
                if (errno == ERANGE || errno == EINVAL)
                    throw LibcException("Invalid baud value");
                break;

            case 'P':
                uart_options.port = optarg;
                break;

            case 'U':
                parse_uart_settings(optarg);
                break;

            case 'R':
                tcpip_options.port = strtol(optarg, nullptr, 10);
                if (errno == ERANGE || errno == EINVAL)
                    throw LibcException("Invalid TCP/IP port");
                break;

            case 'S':
                pty_options.shell = optarg;
                break;

            case '?':
                throw FortunaException("Invalid switch.");
        }

        if (optind > argc)
            print_help(EXIT_FAILURE);
    }
}

[[ noreturn ]] void Options::print_help(int exit_status)
{
    std::cout << "    -c, --communication-mode        One of \"echo\", \"uart\", \"i2c\", \"spi\", \"tcpip\", \"pty\", \"debug\"\n";
    std::cout << "    -w, --window                    Window mode (as opposed to the default, which is full screen)\n";
    std::cout << "    -g, --graphics                  Start in graphics mode (40 columns)\n";
    std::cout << "    -d, --debug-comm                Print all bytes that entered or exited the terminal\n";
    std::cout << "Options valid for `uart`:\n";
    std::cout << "    -P, --serial-port               Serial port (default: /dev/serial0)\n";
    std::cout << "    -B, --baud                      Baud speed for UART (default: 57600)\n";
    std::cout << "    -U, --uart-settings             Data bits, parity, stop bits (default: 8N1)\n";
    std::cout << "Options valid for `tcpip`:\n";
    std::cout << "    -R, --tcpip-port                TCP/IP port (default: 8076)\n";
    std::cout << "Options valid for `pty`:\n";
    std::cout << "    -S, --shell                     Shell executable (default: /bin/sh)\n";
    exit(exit_status);
}

void Options::parse_uart_settings(std::string const &opt)
{
    if (opt.size() != 3)
        throw FortunaException("Invalid UART settings");
    if (opt[0] != '8')
        throw FortunaException("Invalid UART settings: only 8-bit communication is supported.");
    uart_options.stop_bits = (uint8_t) (opt[1] - '0');
    uart_options.parity = opt[2];
}
