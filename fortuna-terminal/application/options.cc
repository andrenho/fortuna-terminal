#include "options.hh"
#include "common/exceptions/libcexception.hh"
#include "common/exceptions/fortunaexception.hh"

#include <cstring>
#include <getopt.h>

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
                // spi
                { "spi-speed",          required_argument, nullptr, 's' },
                { "delay",              required_argument, nullptr, 'D' },
                // tcp/ip
                { "tcpip-port",         required_argument, nullptr, 'R' },
                // shell
                { "shell",              required_argument, nullptr, 'S' },
                // end
                { nullptr, 0, nullptr, 0 },
        };

        c = getopt_long(argc, argv, "c:hwP:B:U:R:S:s:D:dgf", long_options, &option_index);
        if (c == -1)
            break;

        switch (c) {
            case 0:
                printf("option %s", long_options[option_index].name);
                if (optarg)
                    printf(" with arg %s", optarg);
                printf("\n");
                break;

            case 'h':
                print_help(EXIT_SUCCESS);
                break;

            case 'c': {
                if (strcmp(optarg, "echo") == 0)
                    comm_type = CommType::Echo;
                else if (strcmp(optarg, "echo-xchg") == 0)
                    comm_type = CommType::EchoXchg;
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
                mode = Mode::Graphics;
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

            case 's':
                spi_options.speed = strtoll(optarg, nullptr, 10);
                if (errno == ERANGE || errno == EINVAL)
                    throw LibcException("Invalid SPI speed");
                break;

            case 'D': {
                spi_options.delay = std::chrono::microseconds(strtoll(optarg, nullptr, 10));
                if (errno == ERANGE || errno == EINVAL)
                    throw LibcException("Invalid SPI speed");
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
    printf("    -c, --communication-mode        One of \"uart\", \"i2c\", \"spi\", \"tcpip\", \"pty\", \"debug\", \"echo\", \"echo-xchg\"\n");
    printf("    -w, --window                    Window mode (as opposed to the default, which is full screen)\n");
    printf("    -g, --graphics                  Start in graphics mode (40 columns)\n");
    printf("    -d, --debug-comm                Print all bytes that entered or exited the terminal\n");
    printf("Options valid for `uart`:\n");
    printf("    -P, --serial-port               Serial port (default: /dev/serial0)\n");
    printf("    -B, --baud                      Baud speed for UART (default: 57600)\n");
    printf("    -U, --uart-settings             Data bits, parity, stop bits (default: 8N1)\n");
    printf("Options valid for `spi`:\n");
    printf("    -s, --spi-speed                 SPI speed in hz (default: 1000000)\n");
    printf("    -D, --delay                     Delay between bytes, in microseconds (default: 10)");
    printf("Options valid for `tcpip`:\n");
    printf("    -R, --tcpip-port                TCP/IP port (default: 8076)\n");
    printf("Options valid for `pty`:\n");
    printf("    -S, --shell                     Shell executable (default: /bin/sh)\n");
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
