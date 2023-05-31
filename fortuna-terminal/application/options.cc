#include "options.hh"
#include "common/exceptions/libcexception.hh"
#include "common/exceptions/fortunaexception.hh"

#include <cstring>
#include <getopt.h>

using namespace std::string_literals;

Options::Options(int argc, char* argv[])
{
    int c;

    while (true) {
        int option_index = 0;
        static struct option long_options[] = {
                { "communication-mode", required_argument, nullptr, 'c' },
                { "window",             no_argument,       nullptr, 'w' },
                { "debug",              required_argument, nullptr, 'd' },
                { "debug-time",         no_argument,       nullptr, 't' },
                { "welcome",            no_argument,       nullptr, 'W' },
                { "readbuf-sz",         required_argument, nullptr, 'b' },
                // serial
                { "serial-port",        required_argument, nullptr, 'P' },
                { "baud",               required_argument, nullptr, 'B' },
                { "uart-settings",      required_argument, nullptr, 'U' },
                // spi
                { "spi-speed",          required_argument, nullptr, 's' },
                { "delay",              required_argument, nullptr, 'D' },
                // spi
                { "address",            required_argument, nullptr, 'a' },
                // tcp/ip
                { "tcpip-port",         required_argument, nullptr, 'R' },
                // shell
                { "shell",              required_argument, nullptr, 'S' },
                // end
                { nullptr, 0, nullptr, 0 },
        };

        c = getopt_long(argc, argv, "c:hwP:B:U:R:S:s:D:a:d:tW", long_options, &option_index);
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
                else if (strcmp(optarg, "echo-num") == 0)
                    comm_type = CommType::EchoNumeric;
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

            case 'W':
                welcome_message = false;
                break;

            case 't':
                debug_time = true;
                break;

            case 'd':
                debug_verbosity = static_cast<DebugVerbosity>(strtol(optarg, nullptr, 10));
                if (debug_verbosity < 1 || debug_verbosity >= MAX_DEBUG_VERBOSITY)
                    throw FortunaException("Debug verbosity must be between 1 and "s + std::to_string(MAX_DEBUG_VERBOSITY));
                break;

            case 'b':
                readbuf_sz = strtol(optarg, nullptr, 10);
                if (errno == ERANGE || errno == EINVAL)
                    throw LibcException("Invalid baud value");
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

            case 'D':
                spi_options.delay = std::chrono::microseconds(strtoll(optarg, nullptr, 10));
                if (errno == ERANGE || errno == EINVAL)
                    throw LibcException("Invalid SPI delay");
                break;

            case 'a':
                i2c_options.address = strtoll(optarg, nullptr, 16);
                if (errno == ERANGE || errno == EINVAL || i2c_options.address > 126)
                    throw LibcException("Invalid I²C address");
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

    Debug::initialize(debug_verbosity);
}

[[ noreturn ]] void Options::print_help(int exit_status)
{
    printf("    -c, --communication-mode        One of \"uart\", \"i2c\", \"spi\", \"tcpip\", \"pty\", \"debug\", \"echo\", \"echo-num\"\n");
    printf("    -w, --window                    Window mode (as opposed to the default, which is full screen)\n");
    printf("    -t, --debug-time                Show timing information on the screen\n");
    printf("    -d, --debug                     Show debugging information (1: normal (default), 2: info, 3: debug, 4: all communication bytes)\n");
#ifdef COMM_UART
    printf("Options valid for `uart`:\n");
    printf("    -P, --serial-port               Serial port (default: /dev/serial0)\n");
    printf("    -B, --baud                      Baud speed for UART (default: 500000)\n");
    printf("    -U, --uart-settings             Data bits, parity, stop bits (default: 8N1)\n");
#endif
#ifdef COMM_SPI
    printf("Options valid for `spi`:\n");
    printf("    -s, --spi-speed                 SPI speed in hz (default: 200000)\n");
    printf("    -D, --delay                     Delay between bytes, in microseconds (default: 0)");
#endif
#ifdef COMM_SPI
    printf("Options valid for `i2c`:\n");
    printf("    -a, --address                   I²C address (default: 0x3b)\n");
#endif
    printf("Options valid for `tcpip`:\n");
    printf("    -R, --tcpip-port                TCP/IP port (default: 8076)\n");
#ifndef _WIN32
    printf("Options valid for `pty`:\n");
    printf("    -S, --shell                     Shell executable (default: /bin/sh)\n");
#endif
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
