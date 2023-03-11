#ifndef TERMINAL_HH_
#define TERMINAL_HH_

#include <chrono>
#include <functional>
#include <thread>
#include <vector>

#include "lib/SDL2-windows/include/SDL2/SDL.h"

#include "scene/scene.hh"
#include "common/syncqueue.hh"
#include "common/noncopyable.hh"
#include "common/execution.hh"
#include "application/options.hh"
#include "env/protocol/protocol.hh"
#include "texturemanager.hh"
#include "painters/textpainter.hh"
#include "painters/graphicspainter.hh"

using namespace std::chrono_literals;

class Terminal : NonCopyable {
public:
    explicit Terminal(TerminalOptions terminal_options);
    ~Terminal();

    void            draw(Scene const& scene) const;
    void            resize_window(Scene const& scene);

    ExecutionStatus process_user_events(Events& events);
    ExecutionStatus wait_for_enter();

    void            set_mouse_active(bool value);
    void            set_mouse_register_move(bool mouse_register_move) { mouse_register_move_ = mouse_register_move; }
    void            set_joystick_emulation(bool value) { joystick_emulation_ = value; }

private:
    std::unique_ptr<SDL_Window, std::function<void(SDL_Window*)>> window_;
    std::unique_ptr<SDL_Renderer, std::function<void(SDL_Renderer*)>> renderer_;

    std::unique_ptr<TextPainter>     text_painter_;
    std::unique_ptr<GraphicsPainter> graphics_painter_;

    int win_w_ = 800, win_h_ = 600;

    bool window_mode_;

    void beep();

    void add_keyboard_event(Events& events, bool is_down, SDL_KeyboardEvent key);

    bool mouse_active_ = false;
    bool mouse_register_move_ = false;
    bool joystick_emulation_ = false;

    static constexpr const char* emulated_keys = "XxZzSsAaQqWw\r\n\t";

    void print_renderer_info();
};

#endif //TERMINAL_HH_
