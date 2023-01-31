#include "uart.hh"

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
