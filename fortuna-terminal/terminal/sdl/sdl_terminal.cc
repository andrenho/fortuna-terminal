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

        switch (ev.type) {

            case SDL_QUIT:
                running_ = false;
                break;

            case SDL_TEXTINPUT: {
                auto event = OutputEvent { OutputEventType::TextInput };
                memcpy(event.text_input, ev.text.text, SDL_TEXTINPUTEVENT_TEXT_SIZE);
                output_queue.push(event);
                break;
            }

            case SDL_KEYDOWN:
                add_keyboard_event(true, ev.key, output_queue);
                break;

            case SDL_KEYUP:
                add_keyboard_event(false, ev.key, output_queue);
                break;
        }

    }
}

void SDL_Terminal::add_keyboard_event(bool is_down, SDL_KeyboardEvent key, OutputQueue &output_queue)
{
    if (is_down && key.keysym.sym == SDLK_q && (key.keysym.mod & KMOD_CTRL))
        running_ = false;

    OutputEvent output_event { is_down ? OutputEventType::Keydown : OutputEventType::Keyup };
    if (key.keysym.sym >= 32 && key.keysym.sym < 127) {
        output_event.key_code.key_type = KeyType::Regular;
        output_event.key_code.code = (uint8_t) key.keysym.sym;
    } else {
        output_event.key_code.key_type = KeyType::Special;
        switch (key.keysym.sym) {
            case SDLK_ESCAPE: output_event.key_code.special_key = SpecialKey::Esc; break;
            case SDLK_F1: output_event.key_code.special_key = SpecialKey::F1; break;
            case SDLK_F2: output_event.key_code.special_key = SpecialKey::F2; break;
            case SDLK_F3: output_event.key_code.special_key = SpecialKey::F3; break;
            case SDLK_F4: output_event.key_code.special_key = SpecialKey::F4; break;
            case SDLK_F5: output_event.key_code.special_key = SpecialKey::F5; break;
            case SDLK_F6: output_event.key_code.special_key = SpecialKey::F6; break;
            case SDLK_F7: output_event.key_code.special_key = SpecialKey::F7; break;
            case SDLK_F8: output_event.key_code.special_key = SpecialKey::F8; break;
            case SDLK_F9: output_event.key_code.special_key = SpecialKey::F9; break;
            case SDLK_F10: output_event.key_code.special_key = SpecialKey::F10; break;
            case SDLK_F11: output_event.key_code.special_key = SpecialKey::F11; break;
            case SDLK_F12: output_event.key_code.special_key = SpecialKey::F12; break;
            case SDLK_TAB: output_event.key_code.special_key = SpecialKey::Tab; break;
            case SDLK_CAPSLOCK: output_event.key_code.special_key = SpecialKey::CapsLock; break;
            case SDLK_APPLICATION: output_event.key_code.special_key = SpecialKey::Win; break;
            case SDLK_INSERT: output_event.key_code.special_key = SpecialKey::Insert; break;
            case SDLK_HOME: output_event.key_code.special_key = SpecialKey::Home; break;
            case SDLK_END: output_event.key_code.special_key = SpecialKey::End; break;
            case SDLK_PAGEUP: output_event.key_code.special_key = SpecialKey::PageUp; break;
            case SDLK_PAGEDOWN: output_event.key_code.special_key = SpecialKey::PageDown; break;
            case SDLK_UP: output_event.key_code.special_key = SpecialKey::Up; break;
            case SDLK_DOWN: output_event.key_code.special_key = SpecialKey::Down; break;
            case SDLK_LEFT: output_event.key_code.special_key = SpecialKey::Left; break;
            case SDLK_RIGHT: output_event.key_code.special_key = SpecialKey::Right; break;
            case SDLK_RETURN: case SDLK_KP_ENTER: output_event.key_code.special_key = SpecialKey::Enter; break;
            case SDLK_BACKSPACE: output_event.key_code.special_key = SpecialKey::Backspace; break;
            case SDLK_DELETE: output_event.key_code.special_key = SpecialKey::Delete; break;
            case SDLK_PRINTSCREEN: output_event.key_code.special_key = SpecialKey::PrintScreen; break;
            case SDLK_PAUSE: output_event.key_code.special_key = SpecialKey::PauseBreak; break;
            default: return;
        }
    }
    output_event.key_code.shift = (key.keysym.mod & KMOD_SHIFT) != 0;
    output_event.key_code.control = (key.keysym.mod & KMOD_CTRL) != 0;
    output_event.key_code.alt = (key.keysym.mod & KMOD_ALT) != 0;
    output_queue.push(output_event);
}

void SDL_Terminal::draw(const Scene &scene) const
{
    sdl_painter.draw_deep_background();
    sdl_painter.draw_background(scene.text_layer);
    sdl_painter.draw_text(scene.text_layer);

    sdl_painter.present();
}