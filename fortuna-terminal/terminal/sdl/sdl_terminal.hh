#ifndef SDL_TERMINAL_HH_
#define SDL_TERMINAL_HH_

#include "../terminal.hh"

class SDL_Terminal : public Terminal
{
public:
    explicit SDL_Terminal(bool window_mode)
        : window_mode_(window_mode) {}
    ~SDL_Terminal() override;

    void initialize() override;
    void do_events(OutputQueue &output_queue) override;
    void draw(const Scene &scene) const override;

private:
    void print_video_details() const;
    void print_renderer_details(bool selected) const;

    void draw_background(TextLayer const& layer) const;

    bool window_mode_;
    bool initialized_ = false;
    struct SDL_Window* window_ = nullptr;
    struct SDL_Renderer* renderer_ = nullptr;

    static const int GRAPHICS_W = 256;
    static const int GRAPHICS_H = 256;
    static const int WINDOWED_ZOOM = 3;
};

#endif //SDL_TERMINAL_HH_
