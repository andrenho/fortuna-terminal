#ifndef FORTUNA_HH_
#define FORTUNA_HH_

#include "protocol.hh"

class Fortuna : public Protocol {
public:
    Fortuna(std::unique_ptr<CommunicationModule> comm, SyncQueue<SceneEvent> &scene_queue, unsigned int scene_n, Size size);

    void run() override;
    void do_events(SyncQueue<FP_Message> &event_queue) override;
    void finalize_threads() override;
};

#endif //FORTUNA_HH_
