#include "echo.hh"

std::vector<uint8_t> Echo::read_for(Duration duration)
{
    std::this_thread::sleep_for(duration);

    std::vector<uint8_t> r;
    buffer_.pop_all_into_noblock(r);
    return r;
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