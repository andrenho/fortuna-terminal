#include "terminal.hh"

#include "../exceptions/sdlexception.hh"
#include "exceptions/fortunaexception.hh"

#include <iostream>
#include <cstring>

using namespace std::string_literals;

#define JOY_THRESHOLD 1000

Terminal::Terminal(TerminalOptions terminal_options)
    : window_mode_(terminal_options.window_mode)
{
    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO | SDL_INIT_JOYSTICK) < 0)
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

    print_renderer_info();

    texture_manager_ = std::make_unique<TextureManager>(renderer_.get(), 2, 512);

    text_painter_ = std::make_unique<TextPainter>(renderer_.get());
    sprite_painter_ = std::make_unique<SpritePainter>(renderer_.get());
}

Terminal::~Terminal()
{
    texture_manager_.reset();
    sprite_painter_.reset();
    text_painter_.reset();
    renderer_.reset();
    window_.reset();
    SDL_Quit();
}

void Terminal::do_events(Protocol& protocol, bool *quit)
{
    SDL_Event e;
    while (SDL_PollEvent(&e)) {
        if (e.type == SDL_QUIT || (e.type == SDL_KEYDOWN && e.key.keysym.sym == SDLK_F12 && e.key.keysym.mod & KMOD_CTRL))
            *quit = true;

        else if (e.type == SDL_TEXTINPUT) {
            if (joystick_emulation_ && strlen(e.text.text) == 1 && strchr(emulated_keys, e.text.text[0]) != nullptr)
                break;
            protocol.event_text_input(e.text.text);
        }

        else if (e.type == SDL_KEYDOWN) {
            add_keyboard_event(protocol, true, e.key);
        }

        else if (e.type == SDL_KEYUP) {
            add_keyboard_event(protocol, false, e.key);
        }

        else if ((e.type == SDL_MOUSEBUTTONDOWN || e.type == SDL_MOUSEBUTTONUP) && mouse_active_) {
            protocol.event_mouse_button(e.button.button, e.button.x, e.button.y, e.type == SDL_MOUSEBUTTONDOWN);
        }

        else if (e.type == SDL_MOUSEMOTION && mouse_active_ && mouse_register_move_) {
            int button = 0;
            if (e.motion.state & SDL_BUTTON_MMASK)
                button = 2;
            else if (e.motion.state & SDL_BUTTON_RMASK)
                button = 3;
            else if (e.motion.state & SDL_BUTTON_LMASK)
                button = 1;
            protocol.event_mouse_move(button, e.motion.x, e.motion.y);
        }

        else if (e.type == SDL_JOYDEVICEADDED || e.type == SDL_JOYDEVICEREMOVED) {
            SDL_JoystickOpen(e.jdevice.which);
        }

        else if (e.type == SDL_JOYBUTTONDOWN || e.type == SDL_JOYBUTTONUP) {
            protocol.event_joystick(e.jdevice.which, e.jbutton.button, e.type == SDL_JOYBUTTONDOWN);
        }

        else if (e.type == SDL_JOYAXISMOTION) {
            int8_t value = 0;
            if (e.jaxis.value > JOY_THRESHOLD)
                value = 1;
            else if (e.jaxis.value < -JOY_THRESHOLD)
                value = -1;
            protocol.event_joystick_directional(e.jaxis.which, e.jaxis.axis, value);
        }
    }

}

void Terminal::draw(Scene& scene) const
{
    SDL_SetRenderDrawColor(renderer_.get(), 0, 0, 0, SDL_ALPHA_OPAQUE);
    SDL_RenderClear(renderer_.get());

    text_painter_->draw_background(scene.text);
    sprite_painter_->draw(scene.sprites, *texture_manager_);
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
        unsigned int zoom_w = mode.w / (int) terminal_size.w;
        unsigned int zoom_h = mode.h / (int) terminal_size.h;
        unsigned int zoom = std::min(zoom_w, zoom_h);

        win_w_ = (int) terminal_size.w * zoom;
        win_h_ = (int) terminal_size.h * zoom;

        SDL_SetWindowSize(window_.get(), win_w_, win_h_);
        SDL_SetWindowPosition(window_.get(), (mode.w - win_w_) / 2, (mode.h - win_h_) / 2);
    }

    SDL_RenderSetLogicalSize(renderer_.get(), (int) terminal_size.w, (int) terminal_size.h);
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

void Terminal::add_keyboard_event(Protocol& protocol, bool is_down, SDL_KeyboardEvent key)
{
    if (joystick_emulation_) {
        if (key.repeat != 0)
            return;
        switch (key.keysym.sym) {
            case SDLK_UP: protocol.event_joystick_directional(0, 1, is_down ? -1 : 0); return;
            case SDLK_DOWN: protocol.event_joystick_directional(0, 1, is_down ? 1 : 0); return;
            case SDLK_LEFT: protocol.event_joystick_directional(0, 0, is_down ? -1 : 0); return;
            case SDLK_RIGHT: protocol.event_joystick_directional(0, 0, is_down ? 1 : 0); return;
            case SDLK_x: protocol.event_joystick(0, 1, is_down); return;
            case SDLK_z: protocol.event_joystick(0, 2, is_down); return;
            case SDLK_s: protocol.event_joystick(0, 0, is_down); return;
            case SDLK_a: protocol.event_joystick(0, 3, is_down); return;
            case SDLK_RETURN: protocol.event_joystick(0, 9, is_down); return;
            case SDLK_TAB: protocol.event_joystick(0, 8, is_down); return;
            case SDLK_q: protocol.event_joystick(0, 4, is_down); return;
            case SDLK_w: protocol.event_joystick(0, 5, is_down); return;
        }
    }

    KeyMod key_modifiers {
        (key.keysym.mod & KMOD_SHIFT) != 0,
        (key.keysym.mod & KMOD_CTRL) != 0,
        (key.keysym.mod & KMOD_ALT) != 0,
    };

    if (key.keysym.sym >= 32 && key.keysym.sym < 127) {
        protocol.event_key((uint8_t) key.keysym.sym, is_down, key_modifiers);
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

void Terminal::set_mouse_active(bool value)
{
    mouse_active_ = value;
    SDL_ShowCursor(mouse_active_ ? SDL_ENABLE : SDL_DISABLE);
}

void Terminal::print_renderer_info()
{
    SDL_RendererInfo info;
    SDL_GetRendererInfo(renderer_.get(), &info);
    std::cout << "Renderer:\n";
    std::cout << "  Name: " << info.name << "\n";
    std::cout << "  Flags: ";
    if (info.flags & SDL_RENDERER_SOFTWARE)
        std::cout << "software ";
    if (info.flags & SDL_RENDERER_ACCELERATED)
        std::cout << "accelerated ";
    if (info.flags & SDL_RENDERER_PRESENTVSYNC)
        std::cout << "vsync ";
    if (info.flags & SDL_RENDERER_TARGETTEXTURE)
        std::cout << "target_texture ";
    std::cout << "\n";
    std::cout << "  Max texture width: " << info.max_texture_width << "\n";
    std::cout << "  Max texture height: " << info.max_texture_height << "\n";
    std::cout << "  Texture formats:\n";
    for (size_t i = 0; i < info.num_texture_formats; ++i)
        std::cout << "    " << SDL_GetPixelFormatName(info.texture_formats[i]) << "\n";
}
