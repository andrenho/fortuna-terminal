#include "sdl_terminal.hh"

#include <SDL2/SDL.h>
#include <iostream>

#include "../debug.hh"

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


void SDL_Terminal::print_renderer_details(bool selected) const
{
    if (!selected) {
        std::cout << "SDL_RENDER_DRIVER available: ";
        for (int i = 0; i < SDL_GetNumRenderDrivers(); ++i) {
            SDL_RendererInfo info;
            SDL_GetRenderDriverInfo(i, &info);
            std::cout << info.name << " ";
        }
    } else {
        SDL_RendererInfo info;
        SDL_GetRendererInfo( renderer, &info );
        std::cout << "SDL_RENDER_DRIVER selected: " << info.name;
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
    initialized = true;

    if (debug_mode)
        std::cout << "SDL_VIDEODRIVER selected: " << SDL_GetCurrentVideoDriver() << "\n";

    SDL_DisplayMode mode;
    SDL_GetDesktopDisplayMode(0, &mode);
    std::cout <<"Desktop size is " << mode.w << " x " << mode.h << "\n";

    window = SDL_CreateWindow(
            "Fortuna-3 emulator",
            SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED,
            mode.w, mode.h,
            SDL_WINDOW_OPENGL
    );
    if (!window) {
        std::cerr << "SDL_CreateWindow(): " << SDL_GetError() << "\n";
        exit(EXIT_FAILURE);
    }
    SDL_ShowCursor(SDL_DISABLE);

    if (debug_mode)
        print_renderer_details(false);

    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
    if (!renderer) {
        std::cerr << "SDL_CreateRenderer(): " << SDL_GetError() << "\n";
        exit(EXIT_FAILURE);
    }

    if (debug_mode)
        print_renderer_details(true);
}

void SDL_Terminal::update()
{
    SDL_Event ev;
    while (SDL_PollEvent(&ev)) {
        if ((ev.type == SDL_QUIT) || (ev.type == SDL_KEYDOWN && ev.key.keysym.sym == SDLK_ESCAPE))
            running_ = false;
    }

    // deep background
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, SDL_ALPHA_OPAQUE);
    SDL_RenderClear(renderer);

    // TODO

    SDL_RenderPresent(renderer);
}

SDL_Terminal::~SDL_Terminal()
{
    if (renderer)
        SDL_DestroyRenderer(renderer);
    if (window)
        SDL_DestroyWindow(window);
    if (initialized)
        SDL_Quit();
}


