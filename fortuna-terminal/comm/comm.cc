#include "comm.hh"

std::optional<uint8_t> CommunicationModule::read_blocking()
{
    auto v = read_blocking(1);
    if (v.empty())
        return {};
    else
        return v.at(0);
}
