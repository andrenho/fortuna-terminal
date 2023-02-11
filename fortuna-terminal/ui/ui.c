#include "ui.h"
#include "error/error.h"
#include "painter.h"
#include "protocol/protocol.h"
#include "../../fortuna-protocol.h"

#include <stdio.h>

#include <SDL2/SDL.h>

#define WINDOWED_ZOOM 3

static bool        initialized_ = false;
static SDL_Window* window_ = NULL;
static bool        running_ = true;

static void print_video_details()
{
    SDL_Init(0);

    printf("Testing video drivers...\n");
    bool drivers[SDL_GetNumVideoDrivers()];
    for (int i = 0; i < SDL_GetNumVideoDrivers(); ++i) {
        drivers[i] = (0 == SDL_VideoInit(SDL_GetVideoDriver(i)));
        SDL_VideoQuit();
    }

    printf("SDL_VIDEODRIVER available: ");
    for (int i = 0; i < SDL_GetNumVideoDrivers(); ++i) {
        printf("%s ", SDL_GetVideoDriver(i));
    }
    printf("\n");

    printf("SDL_VIDEODRIVER usable: ");
    for (int i = 0; i < SDL_GetNumVideoDrivers(); ++i) {
        if(!drivers[i])
            continue;
        printf("%s ", SDL_GetVideoDriver(i));
    }
    printf("\n");
}

int ui_init()
{
    if (options.debug_mode)
        print_video_details();

    if (SDL_Init(SDL_INIT_VIDEO) < 0)
        return ERR_SDL;
    initialized_ = true;

    if (options.debug_mode)
        printf("SDL_VIDEODRIVER selected: %s\n", SDL_GetCurrentVideoDriver());

    int win_w, win_h;
    if (options.window_mode) {
        win_w = GRAPHICS_W * WINDOWED_ZOOM;
        win_h = GRAPHICS_H * WINDOWED_ZOOM;
    } else {
        SDL_DisplayMode mode;
        SDL_GetDesktopDisplayMode(0, &mode);
        printf("Desktop size is %d x %d\n", mode.w, mode.h);

        win_w = mode.w;
        win_h = mode.h;
    }

    window_ = SDL_CreateWindow(
            "Fortuna-3 emulator",
            SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED,
            win_w, win_h,
            SDL_WINDOW_OPENGL
    );
    if (!window_)
        return ERR_SDL;
    SDL_ShowCursor(SDL_DISABLE);

    if (options.debug_mode) {
        printf("SDL_RENDER_DRIVER available: ");
        for (int i = 0; i < SDL_GetNumRenderDrivers(); ++i) {
            SDL_RendererInfo info;
            SDL_GetRenderDriverInfo(i, &info);
            printf("%s ", info.name);
        }
        printf("\n");
    }

    return painter_init(window_);
}

bool ui_running()
{
    return running_;
}

static void add_keyboard_event(bool is_down, SDL_KeyboardEvent key)
{
    FP_Command fp_command;
    fp_command.command = is_down ? FP_EVENT_KEY_PRESS : FP_EVENT_KEY_RELEASE;

    if (is_down && key.keysym.sym == SDLK_q && (key.keysym.mod & KMOD_CTRL))
        running_ = false;

    if (key.keysym.sym >= 32 && key.keysym.sym < 127) {
        fp_command.key.key = key.keysym.sym;
    } else {
        switch (key.keysym.sym) {
            case SDLK_ESCAPE: fp_command.key.special_key = SK_ESC; break;
            case SDLK_F1: fp_command.key.special_key = SK_F1; break;
            case SDLK_F2: fp_command.key.special_key = SK_F2; break;
            case SDLK_F3: fp_command.key.special_key = SK_F3; break;
            case SDLK_F4: fp_command.key.special_key = SK_F4; break;
            case SDLK_F5: fp_command.key.special_key = SK_F5; break;
            case SDLK_F6: fp_command.key.special_key = SK_F6; break;
            case SDLK_F7: fp_command.key.special_key = SK_F7; break;
            case SDLK_F8: fp_command.key.special_key = SK_F8; break;
            case SDLK_F9: fp_command.key.special_key = SK_F9; break;
            case SDLK_F10: fp_command.key.special_key = SK_F10; break;
            case SDLK_F11: fp_command.key.special_key = SK_F11; break;
            case SDLK_F12: fp_command.key.special_key = SK_F12; break;
            case SDLK_TAB: fp_command.key.special_key = SK_TAB; break;
            case SDLK_CAPSLOCK: fp_command.key.special_key = SK_CAPSLOCK; break;
            case SDLK_APPLICATION: fp_command.key.special_key = SK_WIN; break;
            case SDLK_INSERT: fp_command.key.special_key = SK_INSERT; break;
            case SDLK_HOME: fp_command.key.special_key = SK_HOME; break;
            case SDLK_END: fp_command.key.special_key = SK_END; break;
            case SDLK_PAGEUP: fp_command.key.special_key = SK_PAGEUP; break;
            case SDLK_PAGEDOWN: fp_command.key.special_key = SK_PAGEDOWN; break;
            case SDLK_UP: fp_command.key.special_key = SK_UP; break;
            case SDLK_DOWN: fp_command.key.special_key = SK_DOWN; break;
            case SDLK_LEFT: fp_command.key.special_key = SK_LEFT; break;
            case SDLK_RIGHT: fp_command.key.special_key = SK_RIGHT; break;
            case SDLK_RETURN: case SDLK_KP_ENTER: fp_command.key.special_key = SK_ENTER; break;
            case SDLK_BACKSPACE: fp_command.key.special_key = SK_BACKSPACE; break;
            case SDLK_DELETE: fp_command.key.special_key = SK_DELETE; break;
            case SDLK_PRINTSCREEN: fp_command.key.special_key = SK_PRINTSCREEN; break;
            case SDLK_PAUSE: fp_command.key.special_key = SK_PAUSEBREAK; break;
            default: return;
        }
    }

    KeyMod key_modifiers;
    key_modifiers.shift = (key.keysym.mod & KMOD_SHIFT) != 0;
    key_modifiers.control = (key.keysym.mod & KMOD_CTRL) != 0;
    key_modifiers.alt = (key.keysym.mod & KMOD_ALT) != 0;
    fp_command.key.mod = key_modifiers;

    protocol_terminal_event(&fp_command);
}

int ui_do_events()
{
    SDL_Event ev;
    while (SDL_PollEvent(&ev)) {

        switch (ev.type) {

            case SDL_QUIT:
                running_ = false;
                break;

            case SDL_TEXTINPUT: {
                protocol_terminal_event(&(FP_Command) {
                        .command = FP_EVENT_KEYSTROKE,
                        .keystroke = (struct KeyStroke) {
                                .length = strlen(ev.text.text),
                                .data = (uint8_t *) ev.text.text,
                        },
                });
                break;
            }

            case SDL_KEYDOWN:
                add_keyboard_event(true, ev.key);
                break;

            case SDL_KEYUP:
                add_keyboard_event(false, ev.key);
                break;
        }

    }

    return 0;
}

int ui_draw(Scene* scene)
{
    text_update_blink(&scene->text);
    return painter_draw(scene);
}

int ui_destroy()
{
    int r = painter_destroy();
    if (r != OK)
        return r;
    if (window_)
        SDL_DestroyWindow(window_);
    if (initialized_)
        SDL_Quit();
    return 0;
}
