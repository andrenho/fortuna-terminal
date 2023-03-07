#include "comm_io.hh"

std::optional<uint8_t> CommIO::read_blocking()
{
    auto v = read_blocking(1);
    if (v.empty())
        return {};
    else
        return v.at(0);
}

void CommIO::write(std::string const &str)
{
    std::vector<uint8_t> vec(str.begin(), str.end());
    write(vec);
}

void CommIO::write(uint8_t byte)
{
    std::vector<uint8_t> vec = { byte };
    write(vec);
}
