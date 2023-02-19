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

void Terminal::get_events(SyncQueue<FP_Message> &event_queue, bool *quit)
{
    SDL_Event e;
    while (SDL_PollEvent(&e)) {
        if (e.type == SDL_QUIT || (e.type == SDL_KEYDOWN && e.key.keysym.sym == SDLK_F12 && e.key.keysym.mod & KMOD_CTRL))
            *quit = true;

        else if (e.type == SDL_TEXTINPUT) {
            FP_Message msg = { FP_EVENT_KEYSTROKE, {} };
            strcpy(msg.keystroke, e.text.text);
            event_queue.emplace(std::move(msg));
        }

        else if (e.type == SDL_KEYDOWN) {
            add_keyboard_event(event_queue, true, e.key);
        }

        else if (e.type == SDL_KEYDOWN) {
            add_keyboard_event(event_queue, false, e.key);
        }
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
    while (true) {
        auto omsg = events.pop_nonblock();
        if (!omsg.has_value())
            break;

        SceneEvent const& scene_event = omsg.value();
        FP_Message const& msg = scene_event.message;

        switch (scene_event.message.command) {
            case FP_GRAPHICAL_MODE:
                current_scene().set_graphical_mode(msg.graphical_mode);
                break;
            case FP_TEXT_PRINT_CHAR:
                current_scene().text.write(msg.chr);
                break;
            case FP_TEXT_SET_CHAR:
                current_scene().text.set(msg.set_char.line, msg.set_char.column, Char {msg.set_char.c, msg.set_char.attrib });
                break;
            case FP_TEXT_PRINT_TEXT:
                current_scene().text.write((const char *) msg.text);
                break;
            case FP_TEXT_SET_POS:
                current_scene().text.move_cursor_to(msg.set_pos.line, msg.set_pos.column);
                break;
            case FP_TEXT_SET_CHAR_ATTRIB:
                current_scene().text.set_attributes(msg.char_attrib);
                break;
            case FP_TEXT_SET_PALETTE:
                break;
            case FP_TEXT_CLEAR_SCREEN:
                current_scene().text.clear_screen();
                break;
            case FP_TEXT_SET_CURSOR_ATTRIB:
                current_scene().text.set_cursor_attributes(msg.cursor_attrib);
                break;
            case FP_TEXT_BEEP:
                beep();
                break;

            case FP_EVENT_KEY_PRESS:
            case FP_EVENT_KEY_RELEASE:
            case FP_EVENT_KEYSTROKE:
            case FP_EVENT_JOYSTICK_PRESS:
            case FP_EVENT_JOYSTICK_RELEASE:
            case FP_EVENT_MOUSE_MOVE:
            case FP_EVENT_MOUSE_CLICK:
                throw FortunaException("Received EVENT from computer");

            case FP_ACTIVATE_MOUSE:
            case FP_RESET:
                throw FortunaException("Message "s + std::to_string(scene_event.message.command) + " not implemented");
                break;
        }
    };
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

void Terminal::show_error(std::exception const &ex, bool* quit)
{
    std::cerr << "\e[1;31m" << ex.what() << "\e[0m" << std::endl;

    for (Scene& scene: scenes_) {
        scene.text.set_color(COLOR_RED);
        scene.text.write("\n"s + ex.what() + "\n-- Press ENTER to continue or Ctrl+F12 to quit --\n");
        scene.text.reset_attributes();
        draw();

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
}

void Terminal::beep()
{
    // TODO
}

void Terminal::add_keyboard_event(SyncQueue<FP_Message> &event_queue, bool is_down, SDL_KeyboardEvent key)
{
    FP_Message fp_message;
    fp_message.command = is_down ? FP_EVENT_KEY_PRESS : FP_EVENT_KEY_RELEASE;

    if (key.keysym.sym >= 32 && key.keysym.sym < 127) {
        fp_message.key.key = key.keysym.sym;
    } else {
        switch (key.keysym.sym) {
            case SDLK_ESCAPE: fp_message.key.special_key = SK_ESC; break;
            case SDLK_F1: fp_message.key.special_key = SK_F1; break;
            case SDLK_F2: fp_message.key.special_key = SK_F2; break;
            case SDLK_F3: fp_message.key.special_key = SK_F3; break;
            case SDLK_F4: fp_message.key.special_key = SK_F4; break;
            case SDLK_F5: fp_message.key.special_key = SK_F5; break;
            case SDLK_F6: fp_message.key.special_key = SK_F6; break;
            case SDLK_F7: fp_message.key.special_key = SK_F7; break;
            case SDLK_F8: fp_message.key.special_key = SK_F8; break;
            case SDLK_F9: fp_message.key.special_key = SK_F9; break;
            case SDLK_F10: fp_message.key.special_key = SK_F10; break;
            case SDLK_F11: fp_message.key.special_key = SK_F11; break;
            case SDLK_F12: fp_message.key.special_key = SK_F12; break;
            case SDLK_TAB: fp_message.key.special_key = SK_TAB; break;
            case SDLK_CAPSLOCK: fp_message.key.special_key = SK_CAPSLOCK; break;
            case SDLK_APPLICATION: fp_message.key.special_key = SK_WIN; break;
            case SDLK_INSERT: fp_message.key.special_key = SK_INSERT; break;
            case SDLK_HOME: fp_message.key.special_key = SK_HOME; break;
            case SDLK_END: fp_message.key.special_key = SK_END; break;
            case SDLK_PAGEUP: fp_message.key.special_key = SK_PAGEUP; break;
            case SDLK_PAGEDOWN: fp_message.key.special_key = SK_PAGEDOWN; break;
            case SDLK_UP: fp_message.key.special_key = SK_UP; break;
            case SDLK_DOWN: fp_message.key.special_key = SK_DOWN; break;
            case SDLK_LEFT: fp_message.key.special_key = SK_LEFT; break;
            case SDLK_RIGHT: fp_message.key.special_key = SK_RIGHT; break;
            case SDLK_RETURN: case SDLK_KP_ENTER: fp_message.key.special_key = SK_ENTER; break;
            case SDLK_BACKSPACE: fp_message.key.special_key = SK_BACKSPACE; break;
            case SDLK_DELETE: fp_message.key.special_key = SK_DELETE; break;
            case SDLK_PRINTSCREEN: fp_message.key.special_key = SK_PRINTSCREEN; break;
            case SDLK_PAUSE: fp_message.key.special_key = SK_PAUSEBREAK; break;
            default: return;
        }
    }

    KeyMod key_modifiers;
    key_modifiers.shift = (key.keysym.mod & KMOD_SHIFT) != 0;
    key_modifiers.control = (key.keysym.mod & KMOD_CTRL) != 0;
    key_modifiers.alt = (key.keysym.mod & KMOD_ALT) != 0;
    fp_message.key.mod = key_modifiers;

    event_queue.emplace(std::move(fp_message));
}
