#include "terminal.hh"

#include "../exceptions/sdlexception.hh"

using namespace std::string_literals;

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

void Terminal::do_events(SyncQueue<FP_Message> &event, bool *quit)
{
    SDL_Event e;
    while (SDL_PollEvent(&e)) {
        if (e.type == SDL_QUIT || (e.type == SDL_KEYDOWN && e.key.keysym.sym == SDLK_F12 && e.key.keysym.mod & KMOD_CTRL))
            *quit = true;
    }

    for (auto& scene : scenes_)
        scene.text.update_blink();
}

void Terminal::draw() const
{
    SDL_SetRenderDrawColor(renderer_.get(), 0, 0, 0, SDL_ALPHA_OPAQUE);
    SDL_RenderClear(renderer_.get());

    text_painter_->draw_background(current_scene().text);
    text_painter_->draw(current_scene().text);

    SDL_RenderPresent(renderer_.get());
}

void Terminal::update_scene(SyncQueue<SceneEvent> &events)
{

}

void Terminal::resize_window()
{
    Size terminal_size = current_scene().terminal_size();

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

#include <iostream>
void Terminal::show_error(std::exception const &ex, bool* quit)
{
    for (Scene& scene: scenes_) {
        scene.text.set_color(COLOR_RED);
        scene.text.write("\n"s + ex.what() + "\n-- Press ENTER to continue or Ctrl+F12 to quit --\n");
        scene.text.reset_attributes();
        draw();

        while (true) {
            SDL_Event e;
            while (SDL_PollEvent(&e)) {
                if (e.type == SDL_QUIT || (e.type == SDL_KEYDOWN && e.key.keysym.sym == SDLK_F12 && e.key.keysym.mod & KMOD_CTRL)) {
                    *quit = true;
                    return;
                } else if (e.type == SDL_KEYDOWN && (e.key.keysym.sym == SDLK_RETURN || e.key.keysym.sym == SDLK_KP_ENTER)) {
                    return;
                }
            }
        }
    }
}
