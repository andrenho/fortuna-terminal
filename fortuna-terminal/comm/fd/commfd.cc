#include "commfd.hh"
#include "common/exceptions/libcexception.hh"

#include <unistd.h>

std::string CommFileDescriptor::exchange(std::string_view data_to_send)
{
    std::string received = read();

    if (!data_to_send.empty())
        write(data_to_send);

    return received;
}

std::string CommFileDescriptor::read()
{
    if (fd_ != INVALID_FILE) {
        std::string rd(readbuf_sz_, 0);
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

void CommFileDescriptor::write(std::string_view data_to_send)
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
    fd_ = INVALID_FILE;
    write_fd_ = {};
}

[[noreturn]] void CommFileDescriptor::on_read_error()
{
    throw LibcException("read");
}
