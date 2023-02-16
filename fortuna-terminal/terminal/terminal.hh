#ifndef TERMINAL_HH_
#define TERMINAL_HH_

#include <vector>
#include <thread>

#include "scene/scene.hh"
#include "../common/syncqueue.hh"
#include "../../fortuna-protocol/fortuna-protocol.h"
#include "common/noncopyable.hh"
#include "sceneevent.hh"

class Terminal : NonCopyable {
public:
    Terminal();
    ~Terminal();

    unsigned int add_scene();

    void update_scene(SyncQueue<SceneEvent>& events);

    void do_events(SyncQueue<FP_Message>& event, bool* quit) const;
    void draw() const;

private:
    std::vector<Scene> scenes_;
};

#endif //TERMINAL_HH_
