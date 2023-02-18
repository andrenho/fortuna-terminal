#include "echo.hh"

std::vector<uint8_t> Echo::read_blocking(size_t n)
{
    std::vector<uint8_t> v;
    for (size_t i = 0; i < n; ++i)
        v.push_back(buffer_.pop_block());
    return v;
}

std::vector<uint8_t> Echo::read_for(std::chrono::duration<double> duration)
{
    std::this_thread::sleep_for(duration);

    std::vector<uint8_t> v;
    buffer_.pop_all_into(v);
    return v;
}

void Echo::write(std::vector<uint8_t> const &data)
{
    buffer_.push_all(data);
}
