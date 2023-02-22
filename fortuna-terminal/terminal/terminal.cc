#include "terminal.hh"

#include "../exceptions/sdlexception.hh"
#include "exceptions/fortunaexception.hh"

#include <iostream>

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

void Terminal::do_events(AnsiProtocol& protocol, bool *quit)
{
    SDL_Event e;
    while (SDL_PollEvent(&e)) {
        if (e.type == SDL_QUIT || (e.type == SDL_KEYDOWN && e.key.keysym.sym == SDLK_F12 && e.key.keysym.mod & KMOD_CTRL))
            *quit = true;

        else if (e.type == SDL_TEXTINPUT) {
            protocol.event_text_input(e.text.text);
        }

        else if (e.type == SDL_KEYDOWN) {
            add_keyboard_event(protocol, true, e.key);
        }

        else if (e.type == SDL_KEYDOWN) {
            add_keyboard_event(protocol, false, e.key);
        }
    }

}

void Terminal::draw(Scene const& scene) const
{
    SDL_SetRenderDrawColor(renderer_.get(), 0, 0, 0, SDL_ALPHA_OPAQUE);
    SDL_RenderClear(renderer_.get());

    text_painter_->draw_background(scene.text);
    text_painter_->draw(scene.text);

    SDL_RenderPresent(renderer_.get());
}

void Terminal::resize_window(Scene const& scene)
{
    Size terminal_size = scene.terminal_size();

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

void Terminal::wait_for_enter(bool* quit)
{
    while (true) {
        SDL_Event e;
        while (SDL_PollEvent(&e)) {
            if (e.type == SDL_QUIT || (e.type == SDL_KEYDOWN && e.key.keysym.sym == SDLK_F12 && e.key.keysym.mod & KMOD_CTRL)) {
                if (quit)
                    *quit = true;
                return;
            } else if (e.type == SDL_KEYDOWN && (e.key.keysym.sym == SDLK_RETURN || e.key.keysym.sym == SDLK_KP_ENTER)) {
                return;
            }
        }
    }
}

void Terminal::beep()
{
    // TODO
}

void Terminal::add_keyboard_event(AnsiProtocol& protocol, bool is_down, SDL_KeyboardEvent key)
{
    KeyMod key_modifiers {
        (key.keysym.mod & KMOD_SHIFT) != 0,
        (key.keysym.mod & KMOD_CTRL) != 0,
        (key.keysym.mod & KMOD_ALT) != 0,
    };

    if (key.keysym.sym >= 32 && key.keysym.sym < 127) {
        protocol.event_key(key.keysym.sym & 0xff, is_down, key_modifiers);
    } else {
        SpecialKey special_key;

        switch (key.keysym.sym) {
            case SDLK_ESCAPE: special_key = SpecialKey::ESC; break;
            case SDLK_F1: special_key = SpecialKey::F1; break;
            case SDLK_F2: special_key = SpecialKey::F2; break;
            case SDLK_F3: special_key = SpecialKey::F3; break;
            case SDLK_F4: special_key = SpecialKey::F4; break;
            case SDLK_F5: special_key = SpecialKey::F5; break;
            case SDLK_F6: special_key = SpecialKey::F6; break;
            case SDLK_F7: special_key = SpecialKey::F7; break;
            case SDLK_F8: special_key = SpecialKey::F8; break;
            case SDLK_F9: special_key = SpecialKey::F9; break;
            case SDLK_F10: special_key = SpecialKey::F10; break;
            case SDLK_F11: special_key = SpecialKey::F11; break;
            case SDLK_F12: special_key = SpecialKey::F12; break;
            case SDLK_TAB: special_key = SpecialKey::TAB; break;
            case SDLK_CAPSLOCK: special_key = SpecialKey::CAPSLOCK; break;
            case SDLK_APPLICATION: special_key = SpecialKey::WIN; break;
            case SDLK_INSERT: special_key = SpecialKey::INSERT; break;
            case SDLK_HOME: special_key = SpecialKey::HOME; break;
            case SDLK_END: special_key = SpecialKey::END; break;
            case SDLK_PAGEUP: special_key = SpecialKey::PAGEUP; break;
            case SDLK_PAGEDOWN: special_key = SpecialKey::PAGEDOWN; break;
            case SDLK_UP: special_key = SpecialKey::UP; break;
            case SDLK_DOWN: special_key = SpecialKey::DOWN; break;
            case SDLK_LEFT: special_key = SpecialKey::LEFT; break;
            case SDLK_RIGHT: special_key = SpecialKey::RIGHT; break;
            case SDLK_RETURN: case SDLK_KP_ENTER: special_key = SpecialKey::ENTER; break;
            case SDLK_BACKSPACE: special_key = SpecialKey::BACKSPACE; break;
            case SDLK_DELETE: special_key = SpecialKey::DELETE; break;
            case SDLK_PRINTSCREEN: special_key = SpecialKey::PRINTSCREEN; break;
            case SDLK_PAUSE: special_key = SpecialKey::PAUSEBREAK; break;
            default: return;
        }
        protocol.event_key(special_key, is_down, key_modifiers);
    }
}
