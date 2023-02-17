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
#include "options.hh"
#include "painters/textpainter.hh"

class Terminal : NonCopyable {
public:
    explicit Terminal(TerminalOptions terminal_options);
    ~Terminal();

    unsigned int add_scene();
    void         set_scene(unsigned int n);

    void update_scene(SyncQueue<SceneEvent>& events);

    void do_events(SyncQueue<FP_Message>& event, bool* quit) const;
    void draw() const;

private:
    std::vector<Scene> scenes_;
    std::unique_ptr<SDL_Window, std::function<void(SDL_Window*)>> window_;
    std::unique_ptr<SDL_Renderer, std::function<void(SDL_Renderer*)>> renderer_;

    std::unique_ptr<TextPainter> text_painter_;
    Color backgound_color_ = { 0x1a, 0x1c, 0x2c };

    int current_scene_ = -1;
};

#endif //TERMINAL_HH_
