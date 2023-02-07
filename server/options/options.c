#include "options.h"

#include <getopt.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "error/error.h"

if (communication_mode == CommunicationMode::NotChosen) {
std::cerr << "Communication mode not chosen.\n";
exit(EXIT_FAILURE);
}

static void print_help(int exit_status)
{
    printf("    -c, --communication-mode        One of \"echo\", \"uart\", \"i2c\", \"spi\", \"tcpip\", \"pty\", \"debug\"\n");
    printf("    -p, --protocol                  One of \"fortuna\", \"ansi\" or \"fortuna+ansi\" (default: ansi)\n");
    printf("    -w, --window                    Window mode (as opposed to the default, which is full screen)\n");
    printf("    -d, --debug                     Print lots of debugging information\n");
    printf("    -b, --debug-bytes               Print all bytes received (green) and transmitted (red)\n");
    printf("Options valid for `uart`:\n");
    printf("    -P, --serial-port               Serial port (default: /dev/serial0)\n");
    printf("    -B, --baud                      Baud speed for UART (default: 57600)\n");
    printf("    -U, --uart-settings             Data bits, parity, stop bits (default: 8N1)\n");
    printf("Options valid for `tcpip`:\n");
    printf("    -R, --tcpip-port                TCP/IP port (default: 8076)\n");
    printf("Options valid for `pty`:\n");
    printf("    -S, --shell                     Shell executable (default: /bin/sh)\n");

    exit(exit_status);
}


int options_parse_cmdline(int argc, char *argv[], Options *options)
{
    int c;

    options->comm_mode = CM_NOT_CHOSEN;
    options->protocol = PR_FORTUNA;
    options->debug_mode = false;
    options->debug_bytes = false;
    options->window_mode = false;
    options->serial = (SerialOptions) {
        .baud = 56700,
        .port = "/dev/serial0",
        .parity = 'N',
        .stop_bits = 1,
    };
    options->tcpip = (TCPIPOptions) {
        .port = 8076,
    };
    options->pty = (PTYOptions) {
        .shell = "/bin/sh",
    };

    while (true) {
        int option_index = 0;
        static struct option long_options[] = {
                { "communication-mode", required_argument, NULL, 'c' },
                { "terminal-type",      required_argument, NULL, 't' },
                { "protocol",           required_argument, NULL, 'p' },
                { "window",             no_argument,       NULL, 'w' },
                { "debug",              no_argument,       NULL, 'd' },
                { "debug-bytes",        no_argument,       NULL, 'b' },
                // serial
                { "serial-port",        required_argument, NULL, 'P' },
                { "baud",               required_argument, NULL, 'B' },
                { "uart-settings",      required_argument, NULL, 'U' },
                // tcp/ip
                { "tcpip-port",         required_argument, NULL, 'R' },
                // end
                { NULL, 0, NULL, 0 },
        };

        c = getopt_long(argc, argv, "c:t:p:hdbwP:B:U:R:", long_options, &option_index);
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
                    options->comm_mode = CM_ECHO;
                else if (strcmp(optarg, "uart") == 0)
                    options->comm_mode = CM_UART;
                else if (strcmp(optarg, "i2c") == 0)
                    options->comm_mode = CM_I2C;
                else if (strcmp(optarg, "spi") == 0)
                    options->comm_mode = CM_SPI;
                else if (strcmp(optarg, "tcpip") == 0)
                    options->comm_mode = CM_TCPIP;
                else if (strcmp(optarg, "pty") == 0)
                    options->comm_mode = CM_PTY;
                else if (strcmp(optarg, "debug") == 0)
                    options->comm_mode = CM_DEBUG;
                else {
                    fprintf(stderr, "Unsupported communication mode.\n");
                    exit(EXIT_FAILURE);
                }
                break;
            }

            case 'p': {
                if (strcmp(optarg, "fortuna") == 0)
                    options->protocol = PR_FORTUNA;
                else if (strcmp(optarg, "ansi") == 0)
                    options->protocol = PR_ANSI;
                else if (strcmp(optarg, "fortuna+ansi") == 0)
                    options->protocol = PR_FORTUNA_ANSI;
                else {
                    fprintf(stderr, "Unsupported protocol.\n");
                    exit(EXIT_FAILURE);
                }
                break;
            }

            case 'd':
                options->debug_mode = true;
                break;

            case 'b':
                options->debug_bytes = true;
                break;

            case 'w':
                options->window_mode = true;
                break;

            case 'B':
                options->serial.baud = strtol(optarg, NULL, 10);
                if (errno == ERANGE || errno == EINVAL)
                    return errno;
                break;

            case 'P':
                strncpy(options->serial.port, optarg, PORT_SZ);
                break;

            case 'U':
                parse_uart_settings(optarg);
                break;

            case 'R':
                options->tcpip.port = strtol(optarg, NULL, 10);
                if (errno == ERANGE || errno == EINVAL)
                    return errno;
                break;

            case 'S':
                strncpy(options->pty.shell, optarg, SHELL_SZ);
                break;

            case '?':
                fprintf(stderr, "Invalid switch.\n");
                exit(EXIT_FAILURE);
        }

        if (optind > argc)
            print_help(EXIT_FAILURE);
    }

    validate_options();

    return OK;
}
