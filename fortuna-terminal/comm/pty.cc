#include "pty.hh"

#include <fcntl.h>
#include <unistd.h>
#include <pty.h>
#include <termios.h>

#include <cstdlib>
#include <iostream>

#include "../exceptions/libcexception.hh"
#include "../terminal/scene/layers/text.hh"

PTY::PTY(PTYOptions const& pty_options)
{
    struct winsize winp = { (short unsigned int) Text::Lines_80Columns, (short unsigned int) Text::Columns_80Columns, 0 , 0 };

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

std::vector<uint8_t> PTY::read_blocking(size_t n)
{
    try {
        return FDComm::read_blocking(n);
    } catch (LibcException& e) {
        client_disconnected();
        return { 0, };
    }
}

void PTY::client_disconnected()
{
    FDComm::client_disconnected();
    exit(EXIT_SUCCESS);
}
