#include "sdl_terminal.hh"

#include "SDL2/SDL.h"
#include <iostream>

#include "../../debug.hh"

void SDL_Terminal::print_video_details() const
{
    SDL_Init(0);

    std::cout << "Testing video drivers...\n";
    auto drivers = std::unique_ptr<bool[]>(new bool[SDL_GetNumVideoDrivers()]);
    for (int i = 0; i < SDL_GetNumVideoDrivers(); ++i)
    {
        drivers[i] = (0 == SDL_VideoInit(SDL_GetVideoDriver(i)));
        SDL_VideoQuit();
    }

    std::cout << "SDL_VIDEODRIVER available: ";
    for (int i = 0; i < SDL_GetNumVideoDrivers(); ++i) {
        std::cout << SDL_GetVideoDriver(i) << " ";
    }
    std::cout << "\n";

    std::cout << "SDL_VIDEODRIVER usable: ";
    for (int i = 0; i < SDL_GetNumVideoDrivers(); ++i) {
        if(!drivers[i])
            continue;
        std::cout << SDL_GetVideoDriver(i) << " ";
    }
    std::cout << "\n";
}


void SDL_Terminal::initialize()
{
    if (debug_mode)
        print_video_details();

    if (SDL_Init(SDL_INIT_VIDEO) < 0)
    {
        std::cerr << "SDL_Init(): " << SDL_GetError() << "\n";
        exit(EXIT_FAILURE);
    }
    initialized_ = true;

    if (debug_mode)
        std::cout << "SDL_VIDEODRIVER selected: " << SDL_GetCurrentVideoDriver() << "\n";

    int win_w, win_h;
    if (window_mode_) {
        win_w = GRAPHICS_W * WINDOWED_ZOOM;
        win_h = GRAPHICS_H * WINDOWED_ZOOM;
    } else {
        SDL_DisplayMode mode;
        SDL_GetDesktopDisplayMode(0, &mode);
        std::cout <<"Desktop size is " << mode.w << " x " << mode.h << "\n";

        win_w = mode.w;
        win_h = mode.h;
    }

    window_ = SDL_CreateWindow(
            "Fortuna-3 emulator",
            SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED,
            win_w, win_h,
            SDL_WINDOW_OPENGL
    );
    if (!window_) {
        std::cerr << "SDL_CreateWindow(): " << SDL_GetError() << "\n";
        exit(EXIT_FAILURE);
    }
    SDL_ShowCursor(SDL_DISABLE);

    if (debug_mode) {
        std::cout << "SDL_RENDER_DRIVER available: ";
        for (int i = 0; i < SDL_GetNumRenderDrivers(); ++i) {
            SDL_RendererInfo info;
            SDL_GetRenderDriverInfo(i, &info);
            std::cout << info.name << " ";
        }
        std::cout << "\n";
    }

    sdl_painter.initialize(window_);
}

SDL_Terminal::~SDL_Terminal()
{
    sdl_painter.finalize();
    if (window_)
        SDL_DestroyWindow(window_);
    if (initialized_)
        SDL_Quit();
}

void SDL_Terminal::do_events(OutputQueue &output_queue)
{
    SDL_Event ev;
    while (SDL_PollEvent(&ev)) {
        if ((ev.type == SDL_QUIT) || (ev.type == SDL_KEYDOWN && ev.key.keysym.sym == SDLK_ESCAPE))
            running_ = false;
    }
}

void SDL_Terminal::draw(const Scene &scene) const
{
    sdl_painter.draw_deep_background();
    sdl_painter.draw_background(scene.text_layer);
    sdl_painter.draw_text(scene.text_layer);

    sdl_painter.present();
}