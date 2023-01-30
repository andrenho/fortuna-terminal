#include <iostream>
#include "protocol.hh"
#include "fortunaprotocol.hh"
#include "../util/sync_queue.hh"
#include "../scene/scene.hh"

std::unique_ptr<Protocol> Protocol::make_protocol(const Options &options, OutputQueue &queue, Scene &scene)
{
    switch (options.protocol) {
        case ProtocolType::Fortuna:
            return std::make_unique<FortunaProtocol>(queue, scene);
        default:
            std::cerr << "Unsupported protocol.\n";
            exit(EXIT_FAILURE);
    }
}
