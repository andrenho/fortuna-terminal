#include "fdcomm.hh"
#include "common/exceptions/fortunaexception.hh"

#include <unistd.h>

#include <cstring>

FDComm::~FDComm()
{
    if (fd_ != INVALID_FD)
        close(fd_);
    if (write_fd_ != INVALID_FD)
        close(write_fd_);
}

std::vector<uint8_t> FDComm::read_blocking(size_t n)
{
    if (fd_ == INVALID_FD)
        return {};

    std::vector<uint8_t> data(n);
    int r = read(fd_, data.data(), (int) n);
    if (r < 0)
        on_read_error(strerror(errno));
    else if (r == 0)
        on_read_zero();
    else if (r < (int) n)
        data.resize(n);
    return data;
}

void FDComm::write(std::vector<uint8_t> const &data)
{
    int fd = write_fd_;
    if (fd == INVALID_FD)
        fd = fd_;
    if (fd == INVALID_FD)
        return;

    int n = ::write(fd, data.data(), (int) data.size());
    if (n == 0)
        on_write_error(strerror(errno));
    else if (n < 0)
        on_write_zero();
}

void FDComm::on_rw_zero()
{
    client_disconnected();
}

void FDComm::client_disconnected()
{
    close(fd_);
    fd_ = INVALID_FD;
    if (write_fd_ != INVALID_FD) {
        close(write_fd_);
        write_fd_ = INVALID_FD;
    }
    printf("Client disconnected.\n");
}

void FDComm::on_read_error(std::string const &error)
{
    fprintf(stderr, "Error reading from file descriptor: %s\n", error.c_str());
    client_disconnected();
}

void FDComm::on_write_error(std::string const &error)
{
    fprintf(stderr, "Error writing to file descriptor: %s\n", error.c_str());
    client_disconnected();
}
