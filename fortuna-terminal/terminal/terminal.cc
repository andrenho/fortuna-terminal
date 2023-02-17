#include "terminal.hh"

#include "../exceptions/sdlexception.hh"

Terminal::Terminal(TerminalOptions terminal_options)
    : window_mode_(terminal_options.window_mode)
{
    if (SDL_Init(SDL_INIT_VIDEO) < 0)
        throw SDLException("Error initializing SDL");

    if (!terminal_options.window_mode) {
        SDL_DisplayMode mode;
        SDL_GetDesktopDisplayMode(0, &mode);
        win_w_ = mode.w;
        win_h_ = mode.h;
    }

    window_ = decltype(window_)(
            SDL_CreateWindow("Fortuna-3 emulator",
                SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED,
                win_w_, win_h_,
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
    text_painter_.reset();
    renderer_.reset();
    window_.reset();
    SDL_Quit();
}

unsigned int Terminal::add_scene()
{
    size_t n = scenes_.size();
    scenes_.emplace_back();
    set_scene(n);
    return n;
}

void Terminal::set_scene(unsigned int n)
{
    current_scene_ = (int) n;
    resize_window();
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
    Size terminal_size = scene().terminal_size();

    SDL_SetRenderDrawColor(renderer_.get(), backgound_color_.r, backgound_color_.g, backgound_color_.b, SDL_ALPHA_OPAQUE);
    SDL_Rect r { 0, 0, (int) terminal_size.w, (int) terminal_size.h };
    SDL_RenderFillRect(renderer_.get(), &r);

    text_painter_->draw(scene().text);

    SDL_RenderPresent(renderer_.get());
}

void Terminal::update_scene(SyncQueue<SceneEvent> &events)
{

}

void Terminal::resize_window()
{
    Size terminal_size = scene().terminal_size();

    if (window_mode_) {
        SDL_DisplayMode mode;
        SDL_GetDesktopDisplayMode(0, &mode);

        // find adequate zoom
        unsigned int zoom_w = mode.w / terminal_size.w;
        unsigned int zoom_h = mode.h / terminal_size.h;
        unsigned int zoom = std::min(zoom_w, zoom_h);

        win_w_ = terminal_size.w * zoom;
        win_h_ = terminal_size.h * zoom;

        SDL_SetWindowSize(window_.get(), win_w_, win_h_);
        SDL_SetWindowPosition(window_.get(), (mode.w - win_w_) / 2, (mode.h - win_h_) / 2);
    }

    SDL_RenderSetLogicalSize(renderer_.get(), terminal_size.w, terminal_size.h);
}