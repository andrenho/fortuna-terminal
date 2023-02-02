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
