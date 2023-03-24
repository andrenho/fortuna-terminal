#include "pipes.hh"

#include <cstdio>

Pipes::Pipes(size_t readbuf_sz)
    : CommBuffered(readbuf_sz)
{
    fd_ = STDIN_FILENO;
    write_fd_ = STDOUT_FILENO;
}
