#include "pty.hh"

#include <fcntl.h>
#include <unistd.h>
#include <pty.h>
#include <termios.h>

#include <cstdio>
#include <cstdlib>

#include "common/exceptions/libcexception.hh"
#include "scene/layers/textlayer.hh"

PTY::PTY(PTYOptions const& pty_options, size_t readbuf_sz)
    : CommBuffered(readbuf_sz), shell_(pty_options.shell)
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

    name_ = name;

    // make read non-blocking
    int flags = fcntl(fd_, F_GETFL);
    flags |= O_NONBLOCK;
    flags |= FNDELAY;
    if (fcntl(fd_, F_SETFL, flags) == -1)
        throw LibcException("Could not update_char file descriptor as blocking.");
}

std::string PTY::description() const
{
    return "PTY (shell: " + shell_ + ", name: " + name_ + ")";
}

void PTY::client_disconnected()
{
    exit(0);
}

void PTY::on_read_error()
{
    exit(0);
}
