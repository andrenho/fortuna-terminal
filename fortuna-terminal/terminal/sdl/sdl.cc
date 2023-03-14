#include "sdl.hh"
#include "common/exceptions/sdlexception.hh"
#include "common/exceptions/fortunaexception.hh"

SDL &SDL::get()
{
    static SDL sdl;
    return sdl;
}

void SDL::init(Uint32 flags)
{
    if (SDL_Init(flags) < 0)
        throw SDLException("error initializing SDL");
    get().initialized = true;
}

SDL::~SDL()
{
    if (initialized) {
        for (SDL_Texture* t : textures_)
            SDL_DestroyTexture(t);
        for (SDL_Renderer* r : renderers_)
            SDL_DestroyRenderer(r);
        for (SDL_Window* w : windows_)
            SDL_DestroyWindow(w);
        SDL_Quit();
    }
}

SDL_Window *SDL::create_window(char const *title, int x, int y, int w, int h, Uint32 flags)
{
    check_was_init();
    SDL_Window* window = SDL_CreateWindow(title, x, y, w, h, flags);
    if (!window)
        throw SDLException("Error creating window");
    windows_.push_back(window);
    return window;
}

SDL_Renderer *SDL::create_renderer(SDL_Window *window, int index, Uint32 flags)
{
    check_was_init();
    SDL_Renderer* renderer = SDL_CreateRenderer(window, index, flags);
    if (!renderer)
        throw SDLException("Error creating renderer");
    renderers_.push_back(renderer);
    return renderer;
}

SDL_Texture *SDL::create_texture(SDL_Renderer *renderer, Uint32 format, int access, int w, int h)
{
    check_was_init();
    SDL_Texture* texture = SDL_CreateTexture(renderer, format, access, w, h);
    if (!texture)
        throw SDLException("Error creating texture");
    textures_.push_back(texture);
    return texture;
}

void SDL::check_was_init() const
{
    if (!initialized)
        throw FortunaException("SDL was not initialized");
}

