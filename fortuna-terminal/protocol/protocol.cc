#include <iostream>
#include "protocol.hh"
#include "ansiprotocol.hh"

std::unique_ptr<Protocol> Protocol::make_protocol(Options const& options, InputQueue& input_queue, OutputQueue& output_queue)
{
    switch (options.protocol) {
        case ProtocolType::Ansi:
            return std::make_unique<AnsiProtocol>(input_queue, output_queue);
        default:
            std::cerr << "Unsupported protocol.\n";
            exit(EXIT_FAILURE);
    }
}
