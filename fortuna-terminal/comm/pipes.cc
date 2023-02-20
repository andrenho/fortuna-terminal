#include "pipes.hh"

Pipes::Pipes()
{
    fd_ = STDIN_FILENO;
    write_fd_ = STDOUT_FILENO;
}

