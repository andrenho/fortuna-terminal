#include "echo.hh"

std::string Echo::exchange(std::string_view data_to_send) const
{
    std::string r;
    r.reserve(data_to_send.length());

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
