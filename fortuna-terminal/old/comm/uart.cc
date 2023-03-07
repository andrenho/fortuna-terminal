#include "uart.hh"

#include <cerrno>
#include <cstdio>
#include <cstdlib>
#include <cstring>

#include <fcntl.h>
#include <pthread.h>
#include <termios.h>
#include <unistd.h>

#include "../exceptions/libcexception.hh"
#include "../options.hh"

UART::UART(UartOptions const &uart_options)
{
    fd_ = open(uart_options.port.c_str(), O_RDWR | O_NOCTTY | O_NDELAY);
    if (fd_ < 0)
        throw LibcException("Error opening serial port");
    printf("Serial port initialized.\n");

    fcntl(fd_, F_SETFL, 0);

    struct termios opt;
    if (tcgetattr(fd_, &opt) != 0)
        throw LibcException("Error getting serial attributes");

    if (cfsetspeed(&opt, (speed_t) uart_options.baud) < 0)
        throw LibcException("Error setting serial speed");

    opt.c_cflag |= (CLOCAL | CREAD);  // enable received and set local mode

    opt.c_cflag &= ~CSIZE; /* Mask the character size bits */
    opt.c_cflag |= CS8; /* Select 8 data bits */

    if (uart_options.parity == 'N') {
        opt.c_cflag &= ~PARENB;
    } else if (uart_options.parity == 'O') {
        opt.c_cflag |= PARENB;
        opt.c_cflag |= PARODD;
    } else if (uart_options.parity == 'E') {
        opt.c_cflag |= PARENB;
        opt.c_cflag &= ~PARODD;
    }

    if (uart_options.stop_bits == 2)
        opt.c_cflag |= CSTOPB;
    else
        opt.c_cflag &= ~CSTOPB;

    opt.c_cflag &= ~CRTSCTS;  // disable flow control

    opt.c_iflag &= ~(IGNBRK | BRKINT | PARMRK | ISTRIP | INLCR | IGNCR | ICRNL | IXON);
    opt.c_lflag &= ~(ECHO | ECHONL | ICANON | ISIG | IEXTEN);
    opt.c_oflag &= ~OPOST;

    if (tcsetattr(fd_, TCSANOW, &opt) != 0)
        throw LibcException("Error setting serial attributes");
}
