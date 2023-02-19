#include "fortuna.hh"

Fortuna::Fortuna(std::unique_ptr<CommunicationModule> comm, SyncQueue<SceneEvent> &scene_queue, unsigned int scene_n, Size size)
    : Protocol(std::move(comm), scene_queue, scene_n)
{

}

void Fortuna::run()
{

}

void Fortuna::do_events(SyncQueue<FP_Message> &event_queue)
{

}

void Fortuna::finalize_threads()
{

}

