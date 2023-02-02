#include "uart.hh"
#include "../protocol/protocol.hh"

#include <cerrno>
#include <cstdio>
#include <cstdlib>
#include <cstring>

#include <fcntl.h>
#include <pthread.h>
#include <termios.h>
#include <unistd.h>

#include "../global.hh"

void UART::initialize()
{
    fd = open(options.serial.port.c_str(), O_RDWR | O_NOCTTY | O_NDELAY);
    if (fd < 0) {
        error_message("Error opening serial port: ", true);
        return;
    }
    if (options.debug_mode)
        std::cout << "Serial port initialized.\n";

    fcntl(fd, F_SETFL, 0);

    struct termios opt;
    if (tcgetattr (fd, &opt) != 0)
    {
        error_message("Could not get current serial attributes: ", true);
        return;
    }

    if (cfsetspeed(&opt, (speed_t) options.serial.baud) < 0) {
        error_message("Could not set current serial baud speed: ", true);
        return;
    }

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

    if (tcsetattr (fd, TCSANOW, &opt) != 0)
    {
        error_message("Could not set current serial attributes: ", true);
        return;
    }

    if (options.debug_mode)
        printf("Serial port configured.\n");

}

void UART::run_input_from_device_thread()
{
    while (running_) {
        uint8_t c;
        int n = read(fd, &c, 1);
        if (n < 0)
            error_message("Failure reading from serial", true);
        else
            protocol->input_char(c);
    }
}

void UART::run_output_to_device_thread()
{
    while (running_) {
        uint8_t c = output_queue.dequeue_block();
        if (write(fd, &c, 1) < 0)
            error_message("Failure writing to serial", true);
        tcdrain(fd);
    }
}

void UART::finalize()
{
    CommunicationModule::finalize();

    char c = 'X';
    write(fd, &c, 1);  // this is to unlock the read, so that the thread can join
    usleep(100);

    if (fd != 0)
        close(fd);
}

