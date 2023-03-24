#include "echo.hh"

std::string Echo::thread_exchange(std::string_view data_to_send)
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
