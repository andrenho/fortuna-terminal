#include <iostream>
#include "protocol.hh"
#include "ansiprotocol.hh"

std::unique_ptr<Protocol> Protocol::make_protocol(Options const& options)
{
    switch (options.protocol) {
        case ProtocolType::Ansi:
            return std::make_unique<AnsiProtocol>();
        default:
            std::cerr << "Unsupported protocol.\n";
            exit(EXIT_FAILURE);
    }
}
