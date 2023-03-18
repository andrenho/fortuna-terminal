#include "echo.hh"

std::vector<uint8_t> Echo::read_blocking(size_t n)
{
    std::vector<uint8_t> v;
    v.reserve(n);
    for (size_t i = 0; i < n; ++i)
        v.push_back(buffer_.pop_block());
    return v;
}

void Echo::write(std::vector<uint8_t> const &data)
{
    std::vector<uint8_t> new_data;
    for (uint8_t byte: data) {
        if (byte == 13) {
            new_data.push_back(13);
            new_data.push_back(10);
        } else {
            new_data.push_back(byte);
        }
    }
    buffer_.push_all(new_data);
}

bool Echo::release_locks()
{
    buffer_.push(0x0);
    return true;
}

