#include "uart.h"

#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <fcntl.h>
#include <pthread.h>
#include <termios.h>
#include <unistd.h>

#include "../error/error.h"

static int fd = 0;

int uart_init()
{
    fd = open(options.serial.port, O_RDWR | O_NOCTTY | O_NDELAY);
    if (fd < 0)
        error_check(ERR_LIBC);
    printf("Serial port initialized.\n");

    fcntl(fd, F_SETFL, 0);

    struct termios opt;
    if (tcgetattr(fd, &opt) != 0)
        error_check(ERR_LIBC);

    if (cfsetspeed(&opt, (speed_t) options.serial.baud) < 0)
        error_check(ERR_LIBC);

    opt.c_cflag |= (CLOCAL | CREAD);  // enable received and set local mode

    opt.c_cflag &= ~CSIZE; /* Mask the character size bits */
    opt.c_cflag |= CS8; /* Select 8 data bits */

    if (options.serial.parity == 'N') {
        opt.c_cflag &= ~PARENB;
    } else if (options.serial.parity == 'O') {
        opt.c_cflag |= PARENB;
        opt.c_cflag |= PARODD;
    } else if (options.serial.parity == 'E') {
        opt.c_cflag |= PARENB;
        opt.c_cflag &= ~PARODD;
    }

    if (options.serial.stop_bits == 2)
        opt.c_cflag |= CSTOPB;
    else
        opt.c_cflag &= ~CSTOPB;

    opt.c_cflag &= ~CRTSCTS;  // disable flow control

    opt.c_iflag &= ~(IGNBRK | BRKINT | PARMRK | ISTRIP | INLCR | IGNCR | ICRNL | IXON);
    opt.c_lflag &= ~(ECHO | ECHONL | ICANON | ISIG | IEXTEN);
    opt.c_oflag &= ~OPOST;

    if (tcsetattr(fd, TCSANOW, &opt) != 0)
        error_check(ERR_LIBC);

    printf("Serial port configured.\n");
    return 0;
}

int uart_recv(uint8_t* byte)
{
    if (read(fd, byte, 1) < 0)
        error_check(ERR_LIBC);
    return 0;
}

int uart_send(const uint8_t* data, size_t sz)
{
    if (write(fd, data, sz) < 0)
        error_check(ERR_LIBC);
    return 0;
}

int uart_finalize()
{
    if (fd != 0)
        close(fd);
    return 0;
}
