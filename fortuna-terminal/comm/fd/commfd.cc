#include "commfd.hh"
#include "common/exceptions/libcexception.hh"

#include <unistd.h>

std::vector<uint8_t> CommFileDescriptor::exchange(std::vector<uint8_t> data_to_send)
{
    auto received = read();

    if (!data_to_send.empty())
        write(data_to_send);

    return received;
}

std::vector<uint8_t> CommFileDescriptor::read()
{
    if (fd_ != INVALID_FILE) {
        std::vector<uint8_t> rd(readbuf_sz_, 0);
        int r = ::read(fd_, rd.data(), readbuf_sz_);
        if (r == -1) {
            if (errno == EAGAIN)
                r = 0;
            else
                on_read_error();
        }

        rd.resize(r);
        return rd;
    }

    return {};
}

void CommFileDescriptor::write(std::vector<uint8_t> const& data_to_send)
{
    int fd = this->write_fd_.value_or(this->fd_);
    if (fd != INVALID_FILE) {
        size_t left = data_to_send.size();
        do {
            int r = ::write(fd, data_to_send.data(), data_to_send.size());
            if (r == -1) {
                throw LibcException("write");
            } else if (r == 0) {
                this->client_disconnected();
            } else {
                left -= r;
            }
        } while (left > 0);
    }
}

void CommFileDescriptor::client_disconnected()
{
    close(fd_);
    fd_ = INVALID_FILE;
    if (write_fd_)
        close(*write_fd_);
    write_fd_ = {};
}

[[noreturn]] void CommFileDescriptor::on_read_error()
{
    throw LibcException("read");
}
