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
#include "terminal/scene/sceneevent.hh"
#include "options.hh"
#include "painters/textpainter.hh"
#include "protocol/ansiprotocol.hh"

class Terminal : NonCopyable {
public:
    explicit Terminal(TerminalOptions terminal_options);
    ~Terminal();

    unsigned int add_scene();
    void         set_scene(unsigned int n);

    void update_scene(SyncQueue<SceneEvent>& events);

    void do_events(AnsiProtocol& protocol, bool* quit);
    void draw() const;

    void show_error(std::exception const& e, bool* quit);

    [[nodiscard]] Scene const& current_scene() const { return scenes_.at(current_scene_); }
    [[nodiscard]] unsigned int current_scene_id() const { return current_scene_; }

    [[nodiscard]] size_t total_scene_events() const { return total_scene_events_; }

private:
    std::vector<Scene> scenes_;
    std::unique_ptr<SDL_Window, std::function<void(SDL_Window*)>> window_;
    std::unique_ptr<SDL_Renderer, std::function<void(SDL_Renderer*)>> renderer_;

    std::unique_ptr<TextPainter> text_painter_;

    int win_w_ = 800, win_h_ = 600;

    int current_scene_ = -1;
    bool window_mode_;

    void         resize_window();
    Scene&       current_scene() { return scenes_.at(current_scene_); }

    void beep();

    void add_keyboard_event(AnsiProtocol& protocol, bool is_down, SDL_KeyboardEvent key);

    size_t total_scene_events_ = 0;
};

#endif //TERMINAL_HH_
