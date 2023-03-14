#ifndef TERMINAL_HH_
#define TERMINAL_HH_

#include <chrono>
#include <functional>
#include <thread>
#include <vector>

#include "lib/SDL2-windows/include/SDL2/SDL.h"

#include "scene/scene.hh"
#include "common/syncqueue.hh"
#include "common/types/noncopyable.hh"
#include "common/enums/execution.hh"
#include "application/options.hh"
#include "env/protocol/protocol.hh"
#include "textureatlas.hh"
#include "painters/textpainter.hh"
#include "painters/graphicspainter.hh"
#include "sdl/sdl.hh"
#include "terminalevents.hh"

using namespace std::chrono_literals;

class Terminal : NonCopyable,
     public TerminalEvents {
public:
    explicit Terminal(TerminalOptions terminal_options);

    void draw(Scene const& scene) const;
    void resize_window(Scene const& scene);

    void reset(size_t scene_id) { graphics_painter_->reset(scene_id); }

private:
    SDL_Window* window_ = nullptr;
    SDL_Renderer* renderer_ = nullptr;

    std::unique_ptr<TextPainter>     text_painter_;
    std::unique_ptr<GraphicsPainter> graphics_painter_;

    int win_w_ = 800, win_h_ = 600;

    bool window_mode_;

    void beep();

    void print_renderer_info();
};

#endif //TERMINAL_HH_
