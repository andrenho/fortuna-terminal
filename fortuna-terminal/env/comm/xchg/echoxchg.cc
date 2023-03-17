#include "echoxchg.hh"

std::vector<uint8_t> EchoXchg::exchange(std::vector<uint8_t> const &data)
{
    std::vector<uint8_t> r;
    r.reserve(data.size());

    for (uint8_t c : data) {
        if (c == 13) {
            r.push_back(13);
            r.push_back(10);
        } else if (c != 0xff) {
            r.push_back(c);
        }
    }

    return r;
}
