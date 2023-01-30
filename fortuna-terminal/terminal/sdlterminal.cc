#include "sdlterminal.hh"

#include <SDL2/SDL.h>
#include <iostream>

#include "../debug.hh"

static void print_video_details()
{
    /*
    SDL_Init(0);

    std::cout << "Testing video drivers...\n";
    auto drivers = std::unique_ptr<bool[]>(new bool[SDL_GetNumVideoDrivers()]);
    for (int i = 0; i < SDL_GetNumVideoDrivers(); ++i)
    {
        drivers[i] = (0 == SDL_VideoInit(SDL_GetVideoDriver(i)));
        SDL_VideoQuit();
    }

    std::cout << "SDL_VIDEODRIVER available:";
    for (int i = 0; i < SDL_GetNumVideoDrivers(); ++i) {
        std::cout << SDL_GetVideoDriver(i);
    }
    std::cout << "\n";

    std::cout << "SDL_VIDEODRIVER usable:";
    for (int i = 0; i < SDL_GetNumVideoDrivers(); ++i) {
        if(!drivers[i])
            continue;
        std::cout << SDL_GetVideoDriver(i);
    }
    std::cout << "\n";
     */
}


void SDL_Terminal::initialize()
{
    if (debug_mode)
        print_video_details();
}

void SDL_Terminal::update()
{

}


