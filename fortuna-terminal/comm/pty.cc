#include "pty.hh"
#include "../exceptions/libcexception.hh"

#include <fcntl.h>
#include <unistd.h>
#include <pty.h>
#include <termios.h>

#include <cstdlib>
#include <iostream>

PTY::PTY(PTYOptions const& pty_options, Size terminal_size)
{
    struct winsize winp = { (short unsigned int) terminal_size.h, (short unsigned int) terminal_size.w, 0 , 0 };

    pid_t pid = forkpty(&fd_, NULL, NULL, &winp);
    if (pid < 0) {
        throw LibcException("Error initializing PTY");
    } else if (pid == 0) {
        setenv("LANG", "C", 1);
        setenv("TERM", "ansi", 1);
        // this is the child process that will execute the shell
        if (execl(pty_options.shell.c_str(), pty_options.shell.c_str(), NULL) == -1)
            throw LibcException("Could not initialize shell.");
    }

    // make read blocking
    int flags = fcntl(fd_, F_GETFL);
    flags &= ~O_NONBLOCK;
    if (fcntl(fd_, F_SETFL, flags) == -1)
        throw LibcException("Could not set file descriptor as blocking.");
}

void
PTY::client_disconnected()
{
    close(fd_);
    fd_ = 0;
    std::cout << "PTY client disconnected.\n";
    exit(EXIT_SUCCESS);
}

void PTY::action_on_rw_zero()
{
    client_disconnected();
}

std::vector<uint8_t> PTY::read_blocking(size_t n)
{
    try {
        return FDComm::read_blocking(n);
    } catch (LibcException& e) {
        client_disconnected();
        return { 0, };
    }
}
