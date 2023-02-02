#include <iostream>
#include "protocol.hh"
#include "ansiprotocol.hh"
#include "fortunaprotocol.hh"

std::unique_ptr<Protocol> Protocol::make_protocol(Options const& options, OutputQueue& output_queue)
{
    switch (options.protocol) {
        case ProtocolType::Ansi:
            return std::make_unique<AnsiProtocol>(output_queue);
        case ProtocolType::Fortuna:
            return std::make_unique<FortunaProtocol>(output_queue);
        default:
            std::cerr << "Unsupported protocol.\n";
            exit(EXIT_FAILURE);
    }
}
