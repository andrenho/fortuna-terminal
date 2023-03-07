#include "pty.hh"

#include <fcntl.h>
#include <unistd.h>
#include <pty.h>
#include <termios.h>

#include <cstdlib>
#include <iostream>

#include "../exceptions/libcexception.hh"
#include "../scene/layers/textlayer.hh"

PTY::PTY(PTYOptions const& pty_options)
{
    struct winsize winp = { (short unsigned int) TextLayer::Lines_80Columns, (short unsigned int) TextLayer::Columns_80Columns, 0 , 0 };

    char name[256];
    pid_t pid = forkpty(&fd_, name, nullptr, &winp);
    if (pid < 0) {
        throw LibcException("Error initializing PTY");
    } else if (pid == 0) {
        setenv("LANG", "C", 1);
        setenv("TERM", "ansi", 1);
        // this is the child process that will execute the shell
        if (execl(pty_options.shell.c_str(), pty_options.shell.c_str(), NULL) == -1)
            throw LibcException("Could not initialize shell.");
    }

    std::cout << "Initializing terminal " << name << "." << std::endl;

    // make read blocking
    int flags = fcntl(fd_, F_GETFL);
    flags &= ~O_NONBLOCK;
    if (fcntl(fd_, F_SETFL, flags) == -1)
        throw LibcException("Could not set file descriptor as blocking.");
}

std::vector<uint8_t> PTY::read_blocking(size_t n)
{
    if (fd_ == INVALID_FD)
        return {};

    std::vector<uint8_t> data(n);
    int r = read(fd_, data.data(), n);
    if (r <= 0)
        client_disconnected();
    else if (r < (int) n)
        data.resize(n);
    return data;
}

void PTY::client_disconnected()
{
    FDComm::client_disconnected();
    exit(EXIT_SUCCESS);
}

void PTY::write(std::vector<uint8_t> const &data)
{
    int n = ::write(fd_, data.data(), data.size());
    if (n <= 0)
        client_disconnected();
}
