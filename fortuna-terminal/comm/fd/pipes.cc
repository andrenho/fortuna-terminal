#include "pipes.hh"

#include <cstdio>
#include <unistd.h>

Pipes::Pipes(size_t readbuf_sz)
    : CommFileDescriptor(readbuf_sz)
{
    fd_ = STDIN_FILENO;
    write_fd_ = STDOUT_FILENO;
}
