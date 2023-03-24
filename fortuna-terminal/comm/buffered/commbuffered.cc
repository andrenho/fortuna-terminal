#include "commbuffered.hh"
#include "common/exceptions/libcexception.hh"

#include <unistd.h>

CommBuffered::CommBuffered(size_t readbuf_sz)
    : readbuf_sz_(readbuf_sz)
{
    output_thread_.run_with_wait([this] {
        int fd = write_fd_.value_or(fd_);
        std::vector<uint8_t> v;
        output_queue_.pop_all_into_noblock(v);
        size_t left = v.size();
        do {
            int r = write(fd, v.data(), v.size());
            if (r == -1) {
                throw LibcException("write"); // TODO
            } else if (r == 0) {
                abort();  // TODO
            } else {
                left -= r;
            }
        } while (left > 0);
    });
}

std::string CommBuffered::exchange(std::string_view data_to_send)
{
    std::string rd(readbuf_sz_, 0);
    int r = read(fd_, rd.data(), readbuf_sz_);
    if (r == -1)
        throw LibcException("read");
    else
        rd.resize(r);

    if (!data_to_send.empty()) {
        output_queue_.push_all(data_to_send);
        output_thread_.notify();
    }

    return rd;
}

