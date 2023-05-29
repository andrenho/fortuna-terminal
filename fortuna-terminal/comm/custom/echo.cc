#include "echo.hh"

std::vector<uint8_t> Echo::exchange(std::vector<uint8_t> data_to_send)
{
    std::vector<uint8_t> r;
    r.reserve(data_to_send.size());

    for (char c : data_to_send) {
        if (c == 13) {
            r.push_back(13);
            r.push_back(10);
        } else {
            r.push_back(c);
        }
    }

    return r;
}
