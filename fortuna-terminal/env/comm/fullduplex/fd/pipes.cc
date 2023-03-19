#include "pipes.hh"

#include <cstdio>
#include <unistd.h>

Pipes::Pipes()
{
    fd_ = STDIN_FILENO;
    write_fd_ = STDOUT_FILENO;
}

std::string Pipes::description() const
{
    return "Pipes (stdin/stdout)";
}
