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
#include "protocol/protocol.hh"

class Terminal : NonCopyable {
public:
    explicit Terminal(TerminalOptions terminal_options);
    ~Terminal();

    void do_events(Protocol& protocol, bool* quit);
    void draw(Scene const& scene) const;

    void resize_window(Scene const& scene);

    void wait_for_enter(bool* quit);

    void set_mouse_active(bool value);
    void set_mouse_register_move(bool mouse_register_move) { mouse_register_move_ = mouse_register_move; }

private:
    std::unique_ptr<SDL_Window, std::function<void(SDL_Window*)>> window_;
    std::unique_ptr<SDL_Renderer, std::function<void(SDL_Renderer*)>> renderer_;

    std::unique_ptr<TextPainter> text_painter_;

    int win_w_ = 800, win_h_ = 600;

    bool window_mode_;

    void beep();

    void add_keyboard_event(Protocol& protocol, bool is_down, SDL_KeyboardEvent key);

    size_t total_scene_events_ = 0;

    bool mouse_active_ = false;
    bool mouse_register_move_ = false;
};

#endif //TERMINAL_HH_
