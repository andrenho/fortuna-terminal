#include "uart.hh"
#include "debug.hh"

#include <cerrno>
#include <cstdio>
#include <cstdlib>
#include <cstring>

#include <fcntl.h>
#include <pthread.h>
#include <termios.h>
#include <unistd.h>

void UART::initialize()
{
    fd = open(serial_options_.port.c_str(), O_RDWR | O_NOCTTY | O_NDELAY);
    if (fd < 0) {
        error_message("Error opening serial port: ", true);
        return;
    }
    if (debug_mode)
        std::cout << "Serial port initialized.\n";

    fcntl(fd, F_SETFL, 0);

    struct termios options;
    if (tcgetattr (fd, &options) != 0)
    {
        error_message("Could not get current serial attributes: ", true);
        return;
    }

    if (cfsetspeed(&options, (speed_t) serial_options_.baud) < 0) {
        error_message("Could not set current serial baud speed: ", true);
        return;
    }

    options.c_cflag |= (CLOCAL | CREAD);  // enable received and set local mode

    options.c_cflag &= ~CSIZE; /* Mask the character size bits */
    options.c_cflag |= CS8; /* Select 8 data bits */

    if (serial_options_.parity == 'N') {
        options.c_cflag &= ~PARENB;
    } else if (serial_options_.parity == 'O') {
        options.c_cflag |= PARENB;
        options.c_cflag |= PARODD;
    } else if (serial_options_.parity == 'E') {
        options.c_cflag |= PARENB;
        options.c_cflag &= ~PARODD;
    }

    if (serial_options_.stop_bits == 2)
        options.c_cflag |= CSTOPB;
    else
        options.c_cflag &= ~CSTOPB;

    options.c_cflag &= ~CRTSCTS;  // disable flow control

    options.c_iflag &= ~(IGNBRK | BRKINT | PARMRK | ISTRIP | INLCR | IGNCR | ICRNL | IXON);
    options.c_lflag &= ~(ECHO | ECHONL | ICANON | ISIG | IEXTEN);
    options.c_oflag &= ~OPOST;

    if (tcsetattr (fd, TCSANOW, &options) != 0)
    {
        error_message("Could not set current serial attributes: ", true);
        return;
    }

    if (debug_mode)
        printf("Serial port configured.\n");

}

void UART::run_input_from_device_thread()
{
    while (running_) {
        uint8_t c;
        int n = read(fd, &c, 1);
        if (n < 0)
            error_message("Failure reading from serial", true);
        protocol_.input(c, input_queue_);
    }
}

void UART::run_output_to_device_thread()
{
    while (running_) {
        uint8_t c = output_queue_.dequeue_block();
        if (write(fd, &c, 1) < 0)
            error_message("Failure writing to serial", true);
        tcdrain(fd);
    }
}

void UART::finalize()
{
    char c = 'X';
    write(fd, &c, 1);  // this is to unlock the read, so that the thread can join
    usleep(100);

    if (fd != 0)
        close(fd);
    CommunicationModule::finalize();
}

void UART::error_message(std::string const &msg, bool describe_errno)
{
    std::string pmsg = msg;
    if (describe_errno)
        pmsg += std::string(": ") + strerror(errno) + "\n";
    else
        pmsg += ".\n";

    input_queue_.enqueue({ InputEventType::TextSetColor, Color::RED });
    for (char c: pmsg)
        input_queue_.enqueue({ InputEventType::TextPrintChar, (uint8_t) c });
    input_queue_.enqueue(InputEvent { InputEventType::TextResetFormatting });
}
