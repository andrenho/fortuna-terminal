#ifndef TERMINAL_HH_
#define TERMINAL_HH_

#include <vector>

#include "scene/scene.hh"
#include "../common/syncqueue.hh"
#include "../../fortuna-protocol/fortuna-protocol.h"

class Terminal {
public:
    unsigned int add_scene();

    void update_scene(unsigned int scene_n, FP_Message&& message);

    void do_events(SyncQueue<FP_Message>& event, bool* quit) const;
    void draw() const;

private:
    std::vector<Scene> scenes_;
};

#endif //TERMINAL_HH_
