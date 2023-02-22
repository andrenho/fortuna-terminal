#include "pipes.hh"

#include <cstdio>

Pipes::Pipes()
{
    fd_ = STDIN_FILENO;
    write_fd_ = STDOUT_FILENO;
}

