#include "echo.hh"

std::string Echo::exchange(std::string_view data_to_send) const
{
    return std::string(data_to_send);
}
