#include "fdcomm.hh"
#include "../exceptions/libcexception.hh"
#include "../exceptions/fortunaexception.hh"

#include <unistd.h>

FDComm::~FDComm()
{
    if (fd_ != 0)
        close(fd_);
}

std::vector<uint8_t> FDComm::read_blocking(size_t n)
{
    last_wait_ = n;

    std::vector<uint8_t> data(n);
    int r = read(fd_, data.data(), n);
    if (r < 0)
        throw LibcException("Error reading from file descriptor");
    else if (r == 0)
        action_on_rw_zero();
    else if (r < (int) n)
        data.resize(n);
    return data;
}

void FDComm::write(std::vector<uint8_t> const &data)
{
    int n = ::write(fd_, data.data(), data.size());
    if (n == 0)
        action_on_rw_zero();
    else if (n < 0)
        throw LibcException("Error writing to file descriptor");
}
