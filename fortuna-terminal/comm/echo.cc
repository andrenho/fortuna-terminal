#include "echo.hh"

std::vector<uint8_t> Echo::read_blocking(size_t n) const
{
    return std::vector<uint8_t>();
}

std::vector<uint8_t> Echo::read_for(std::chrono::duration<double> duration) const
{
    return std::vector<uint8_t>();
}

void Echo::write(std::vector<uint8_t> const &data) const
{

}
