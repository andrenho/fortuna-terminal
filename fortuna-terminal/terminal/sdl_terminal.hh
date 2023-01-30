#ifndef SDL_TERMINAL_HH_
#define SDL_TERMINAL_HH_

#include "terminal.hh"

class SDL_Terminal : public Terminal
{
public:
    explicit SDL_Terminal(CommunicationModule& comm, bool window_mode) : Terminal(comm), window_mode_(window_mode) {}
    virtual ~SDL_Terminal();

    void initialize() override;
    void update() override;

private:
    void print_video_details() const;
    void print_renderer_details(bool selected) const;

    bool window_mode_;
    bool initialized_ = false;
    struct SDL_Window* window_ = nullptr;
    struct SDL_Renderer* renderer_ = nullptr;

    static const int GRAPHICS_W = 256;
    static const int GRAPHICS_H = 256;
    static const int WINDOWED_ZOOM = 3;
};

#endif //SDL_TERMINAL_HH_
