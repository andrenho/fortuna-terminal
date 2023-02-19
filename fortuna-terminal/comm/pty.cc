#include "pty.hh"
#include "../common/geometry.hh"
#include "../exceptions/libcexception.hh"

#include <fcntl.h>
#include <unistd.h>
#include <pty.h>
#include <termios.h>

#include <cstdlib>
#include <iostream>

PTY::PTY(PTYOptions const &pty_options, Size terminal_size)
{
    struct winsize winp = { terminal_size.h, terminal_size.w, 0 , 0 };

    pid_t pid = forkpty(&fd_, NULL, NULL, &winp);
    if (pid < 0) {
        throw LibcException("Error initializing PTY");
    } else if (pid == 0) {
        setenv("LANG", "C", 1);
        setenv("TERM", "ansi", 1);
        // this is the child process that will execute the shell
        execl(pty_options.shell.c_str(), pty_options.shell.c_str(), NULL);
    }

    // make read blocking
    int flags = fcntl(fd_, F_GETFL);
    flags &= ~O_NONBLOCK;
    fcntl(fd_, F_SETFL, flags);
}

PTY::~PTY()
{
    if (fd_ != 0)
        close(fd_);
}

std::vector<uint8_t> PTY::read_blocking(size_t n)
{
    return std::vector<uint8_t>();
}

std::vector<uint8_t> PTY::read_for(std::chrono::duration<double> duration)
{
    return std::vector<uint8_t>();
}

void PTY::write(std::vector<uint8_t> const &data)
{
    int n = write(fd_, data.data(), data.size());
    if (n == 0)
        pty_client_disconnected();
    else if (n < 0)
        throw LibcException("Error writing to PTY");
}

void
PTY::client_disconnected()
{
    close(fd_);
    fd_ = 0;
    std::cout << "PTY client disconnected.\n";
    exit(EXIT_SUCCESS);
}