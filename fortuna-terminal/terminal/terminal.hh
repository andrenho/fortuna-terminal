#ifndef TERMINAL_HH_
#define TERMINAL_HH_

#include <functional>
#include <thread>
#include <vector>

#include <SDL2/SDL.h>

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
    std::unique_ptr<SDL_Window, std::function<void(SDL_Window*)>> window_;
    std::unique_ptr<SDL_Renderer, std::function<void(SDL_Renderer*)>> renderer_;
};

#endif //TERMINAL_HH_
