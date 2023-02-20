#include "protocol.hh"
#include "ansiprotocol.hh"
#include "exceptions/fortunaexception.hh"

std::unique_ptr<Protocol>
Protocol::create_unique(ProtocolType protocol_type, std::unique_ptr<CommunicationModule> comm,
                        SyncQueue<SceneEvent> &scene_queue,
                        unsigned int scene_n, Size initial_size)
{
    switch (protocol_type) {
        case ProtocolType::Ansi:
            return std::make_unique<AnsiProtocol>(std::move(comm), scene_queue, scene_n, initial_size);
        case ProtocolType::Fortuna:
            throw FortunaException("Protocol not implemented.");
    }

    throw FortunaException("Invalid protocol.");
}

Protocol::Protocol(std::unique_ptr<CommunicationModule> comm, SyncQueue<SceneEvent> &scene_queue, unsigned int scene_n)
    : comm_(std::move(comm)), scene_queue_(scene_queue), scene_n_(scene_n)
{
}
