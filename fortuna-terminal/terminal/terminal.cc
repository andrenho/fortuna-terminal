#include "terminal.hh"

#include "../exceptions/sdlexception.hh"

Terminal::Terminal(TerminalOptions terminal_options)
{
    if (SDL_Init(SDL_INIT_VIDEO) < 0)
        throw SDLException("Error initializing SDL");

    int win_w = 800, win_h = 600;
    if (!terminal_options.window_mode) {
        SDL_DisplayMode mode;
        SDL_GetDesktopDisplayMode(0, &mode);
        win_w = mode.w;
        win_h = mode.h;
    }

    window_ = decltype(window_)(
            SDL_CreateWindow("Fortuna-3 emulator",
                SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED,
                win_w, win_h,
                SDL_WINDOW_OPENGL),
         [](SDL_Window *w) {
                SDL_DestroyWindow(w);
            });
    if (!window_)
        throw SDLException("Error creating window");

    SDL_ShowCursor(SDL_DISABLE);

    renderer_ = decltype(renderer_)(
        SDL_CreateRenderer(window_.get(), -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC),
        [](SDL_Renderer* r) {
            SDL_DestroyRenderer(r);
        }
    );
    if (!renderer_)
        throw SDLException("Error creating renderer");

    text_painter_ = std::make_unique<TextPainter>(renderer_.get());
}

Terminal::~Terminal()
{
    /*
    renderer_.reset();
    window_.reset();
    SDL_Quit();
     */
}

unsigned int Terminal::add_scene()
{
    scenes_.emplace_back();
    return scenes_.size() - 1;
}

void Terminal::set_scene(unsigned int n)
{
    current_scene_ = (int) n;
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
    // SDL_SetRenderDrawColor(renderer_.get(), backgound_color_.r, backgound_color_.g, backgound_color_, SDL_ALPHA_OPAQUE);
    // SDL_RenderFillRect(renderer_, &(SDL_Rect) {0, 0, GRAPHICS_W, GRAPHICS_H });

    SDL_RenderPresent(renderer_.get());
}

void Terminal::update_scene(SyncQueue<SceneEvent> &events)
{

}

