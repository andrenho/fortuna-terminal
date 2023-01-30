#ifndef SDL_TERMINAL_HH_
#define SDL_TERMINAL_HH_

#include "terminal.hh"

class SDL_Terminal : public Terminal
{
public:
    explicit SDL_Terminal(CommunicationModule& comm) : Terminal(comm) {}
    virtual ~SDL_Terminal();

    void initialize() override;
    void update() override;

private:
    void print_video_details() const;
    void print_renderer_details(bool selected) const;

    bool initialized = false;
    struct SDL_Window* window = nullptr;
    struct SDL_Renderer* renderer = nullptr;
};

#endif //SDL_TERMINAL_HH_
