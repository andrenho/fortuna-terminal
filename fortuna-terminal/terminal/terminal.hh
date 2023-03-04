#ifndef TERMINAL_HH_
#define TERMINAL_HH_

#include <chrono>
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
#include "texturemanager.hh"
#include "terminal/painters/graphicspainter.hh"

using Duration = decltype(std::chrono::high_resolution_clock::now() - std::chrono::high_resolution_clock::now());

using namespace std::chrono_literals;

class Terminal : NonCopyable {
public:
    explicit Terminal(TerminalOptions terminal_options);
    ~Terminal();

    void do_events(Protocol& protocol, bool* quit);
    void draw(Scene& scene) const;

    void resize_window(Scene const& scene);

    void wait_for_enter(bool* quit);

    void set_mouse_active(bool value);
    void set_mouse_register_move(bool mouse_register_move) { mouse_register_move_ = mouse_register_move; }
    void set_joystick_emulation(bool value) { joystick_emulation_ = value; }

    void setup_scene(Scene const& scene);

    void set_frame_duration(Duration duration) { frame_duration_ = duration; }

private:
    std::unique_ptr<SDL_Window, std::function<void(SDL_Window*)>> window_;
    std::unique_ptr<SDL_Renderer, std::function<void(SDL_Renderer*)>> renderer_;

    std::unique_ptr<TextPainter>     text_painter_;
    std::unique_ptr<GraphicsPainter> graphics_painter_;

    int win_w_ = 800, win_h_ = 600;

    bool window_mode_;
    bool show_fps_;

    void beep();

    void add_keyboard_event(Protocol& protocol, bool is_down, SDL_KeyboardEvent key);

    bool mouse_active_ = false;
    bool mouse_register_move_ = false;
    bool joystick_emulation_ = false;

    Duration frame_duration_ = 0ms;

    static constexpr const char* emulated_keys = "XxZzSsAaQqWw\r\n\t";

    void print_renderer_info();
};

#endif //TERMINAL_HH_
