#include "echofortuna.hh"

std::vector<uint8_t> EchoFortuna::read_blocking(size_t n)
{
    return std::vector<uint8_t>();
}

void EchoFortuna::write(std::vector<uint8_t> const &data)
{

}

bool EchoFortuna::release_locks()
{
    return CommunicationModule::release_locks();
}
