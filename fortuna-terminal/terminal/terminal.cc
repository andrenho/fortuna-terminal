#include "terminal.hh"

#include "../exceptions/sdlexception.hh"

Terminal::Terminal()
{
    if (SDL_Init(SDL_INIT_VIDEO) < 0)
        throw SDLException("Error initializing SDL");

    window_ = decltype(window_)(
            SDL_CreateWindow("Fortuna-3 emulator",
                SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED,
                800, 600,
                SDL_WINDOW_OPENGL),
         [](SDL_Window *w) { SDL_DestroyWindow(w); });
    if (!window_)
        throw SDLException("Error creating window");

    renderer_ = decltype(renderer_)(
        SDL_CreateRenderer(window_.get(), -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC),
        [](SDL_Renderer* r) { SDL_DestroyRenderer(r); }
    );
    if (!renderer_)
        throw SDLException("Error creating renderer");
}

Terminal::~Terminal()
{
    renderer_.reset();
    window_.reset();
    SDL_Quit();
}

unsigned int Terminal::add_scene()
{
    scenes_.emplace_back();
    return scenes_.size() - 1;
}

void Terminal::do_events(SyncQueue<FP_Message> &event, bool *quit) const
{
    SDL_Event e;
    while (SDL_PollEvent(&e)) {
        if (e.type == SDL_QUIT || (e.type == SDL_KEYDOWN && e.key.keysym.sym == SDLK_F12))
            *quit = true;
    }
}

void Terminal::draw() const
{
    SDL_RenderPresent(renderer_.get());
}

void Terminal::update_scene(SyncQueue<SceneEvent> &events)
{

}
