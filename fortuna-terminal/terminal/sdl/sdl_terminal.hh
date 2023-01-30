#ifndef SDL_TERMINAL_HH_
#define SDL_TERMINAL_HH_

#include "../terminal.hh"
#include "terminal/sdl/sdl_painter.hh"

class SDL_Terminal : public Terminal
{
public:
    explicit SDL_Terminal(bool window_mode)
        : window_mode_(window_mode) {}
    ~SDL_Terminal() override;

    void initialize() override;
    void do_events(OutputQueue &output_queue) override;
    void draw(const Scene &scene) const override;

    static const int GRAPHICS_W = 256;
    static const int GRAPHICS_H = 256;

private:
    void print_video_details() const;

    bool window_mode_;

    SDL_Painter          sdl_painter;
    bool                 initialized_ = false;
    struct SDL_Window*   window_ = nullptr;

    static const int WINDOWED_ZOOM = 3;
};

#endif //SDL_TERMINAL_HH_
