#ifndef TERMINAL_HH_
#define TERMINAL_HH_

#include <functional>
#include <thread>
#include <vector>

#include <SDL2/SDL.h>

#include "scene/scene.hh"
#include "../common/syncqueue.hh"
#include "common/noncopyable.hh"
#include "options.hh"
#include "painters/textpainter.hh"
#include "protocol/ansiprotocol.hh"

class Terminal : NonCopyable {
public:
    explicit Terminal(TerminalOptions terminal_options);
    ~Terminal();

    void do_events(AnsiProtocol& protocol, bool* quit);
    void draw(Scene const& scene) const;

    void resize_window(Scene const& scene);

    void show_error(std::exception const& e, bool* quit);

private:
    std::unique_ptr<SDL_Window, std::function<void(SDL_Window*)>> window_;
    std::unique_ptr<SDL_Renderer, std::function<void(SDL_Renderer*)>> renderer_;

    std::unique_ptr<TextPainter> text_painter_;

    int win_w_ = 800, win_h_ = 600;

    bool window_mode_;

    void beep();

    void add_keyboard_event(AnsiProtocol& protocol, bool is_down, SDL_KeyboardEvent key);

    size_t total_scene_events_ = 0;
};

#endif //TERMINAL_HH_
