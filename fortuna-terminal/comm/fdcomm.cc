#include "fdcomm.hh"
#include "../exceptions/libcexception.hh"
#include "../exceptions/fortunaexception.hh"

#include <unistd.h>

#include <cstring>
#include <iostream>

FDComm::~FDComm()
{
    if (fd_ != 0)
        close(fd_);
}

std::vector<uint8_t> FDComm::read_blocking(size_t n)
{
    if (fd_ == 0)
        return {};

    std::vector<uint8_t> data(n);
    int r = read(fd_, data.data(), n);
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
    if (fd_ == 0)
        return;

    int n = ::write(fd_, data.data(), data.size());
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
    fd_ = 0;
    std::cout << "Client disconnected." << std::endl;
}

void FDComm::on_read_error(std::string const &error)
{
    std::cerr << "\e[1;31mError reading from file descriptor: " << error << "\e[0m" << std::endl;
    client_disconnected();
}

void FDComm::on_write_error(std::string const &error)
{
    std::cerr << "\e[1;31mError writing to file descriptor: " << error << "\e[0m" << std::endl;
    client_disconnected();
}
