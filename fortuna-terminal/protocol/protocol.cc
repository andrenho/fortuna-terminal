#include <iostream>
#include "protocol.hh"
#include "ansiprotocol.hh"
#include "fortunaprotocol.hh"
#include "global.hh"

std::unique_ptr<Protocol> Protocol::make_protocol()
{
    switch (options.protocol) {
        case ProtocolType::Ansi:
            return std::make_unique<AnsiProtocol>();
        case ProtocolType::Fortuna:
            return std::make_unique<FortunaProtocol>();
        default:
            std::cerr << "Unsupported protocol.\n";
            exit(EXIT_FAILURE);
    }
}

void Protocol::debug_special(std::string const &str) const
{
    if (options.debug_bytes) {
        std::cout << "\e[0;34m(" << str << ")\e[0m ";
    }
}

void Protocol::debug_special(std::string const &str, uint8_t p1) const
{
    if (options.debug_bytes) {
        std::cout << "\e[0;34m(" << str << ", " << (int) p1 << ")\e[0m ";
    }
}

void Protocol::debug_special(std::string const &str, uint8_t p1, uint8_t p2) const
{
    if (options.debug_bytes) {
        std::cout << "\e[0;34m(" << str << ", " << (int) p1 << ", " << (int) p2 << ")\e[0m ";
    }
}
