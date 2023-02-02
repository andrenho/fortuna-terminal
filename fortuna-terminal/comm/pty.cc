#include "pty.hh"

#include <cstdlib>
#include <fcntl.h>
#include <unistd.h>
#include <pty.h>
#include <fcntl.h>

void PTY::initialize()
{
    pid_t pid = forkpty(&master_fd, nullptr, nullptr, nullptr);  // TODO - set winsize
    if (pid == 0) {
        // this is the child process that will execute the shell
        execl(pty_options_.shell.c_str(), (char *) nullptr);
    }

    // make read blocking
    int flags = fcntl(master_fd, F_GETFL);
    flags &= ~O_NONBLOCK;
    fcntl(master_fd, F_SETFL, flags);
}

void PTY::run_input_from_device_thread()
{
    while (running_) {
        uint8_t c;
        int n = read(master_fd, &c, 1);
        if (n < 0)
            error_message("Failure reading from the PTY", true);
        else
            protocol_.input_char(c);
    }
}

void PTY::run_output_to_device_thread()
{
    while (running_) {
        uint8_t c = output_queue_.dequeue_block();
        if (write(master_fd, &c, 1) < 0)
            error_message("Failure writing to the PTY", true);
    }
}

void PTY::finalize()
{
    CommunicationModule::finalize();
    close(master_fd);
}
