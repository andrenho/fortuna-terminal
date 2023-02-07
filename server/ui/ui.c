#include "ui.h"
#include "error/error.h"
#include "painter.h"

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

int ui_init(Options* options)
{
    if (options->debug_mode)
        print_video_details();

    if (SDL_Init(SDL_INIT_VIDEO) < 0)
        return ERR_SDL;
    initialized_ = true;

    if (options->debug_mode)
        printf("SDL_VIDEODRIVER selected: %s\n", SDL_GetCurrentVideoDriver());

    int win_w, win_h;
    if (options->window_mode) {
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

    if (options->debug_mode) {
        printf("SDL_RENDER_DRIVER available: ");
        for (int i = 0; i < SDL_GetNumRenderDrivers(); ++i) {
            SDL_RendererInfo info;
            SDL_GetRenderDriverInfo(i, &info);
            printf("%s ", info.name);
        }
        printf("\n");
    }

    return painter_init(window_, options);
}

bool ui_running()
{
    return running_;
}

int ui_do_events()
{
    SDL_Event ev;
    while (SDL_PollEvent(&ev)) {

        switch (ev.type) {

            case SDL_QUIT:
                running_ = false;
                break;

                /*
            case SDL_TEXTINPUT: {
                for (const char* c = ev.text.text; *c; ++c)
                    protocol->output_key_event(true, *c, {});
                break;
            }

            case SDL_KEYDOWN:
                add_keyboard_event(true, ev.key);
                break;

            case SDL_KEYUP:
                add_keyboard_event(false, ev.key);
                break;
                 */
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
