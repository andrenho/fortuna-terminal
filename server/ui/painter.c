#include "painter.h"
#include "error/error.h"
#include "ui.h"

#include <stdio.h>

static SDL_Renderer* renderer_ = NULL;

int painter_init(SDL_Window *window, Options *options)
{

    renderer_ = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
    if (!renderer_)
        return ERR_SDL;
    SDL_RenderSetLogicalSize(renderer_, GRAPHICS_W, GRAPHICS_H);

    if (options->debug_mode) {
        SDL_RendererInfo info;
        SDL_GetRendererInfo(renderer_, &info );
        printf("SDL_RENDER_DRIVER selected: %s\n", info.name);
    }

    // load_font();

    return 0;
}

static void painter_draw_deep_background()
{
    SDL_SetRenderDrawColor(renderer_, 0, 0, 0, SDL_ALPHA_OPAQUE);
    SDL_RenderClear(renderer_);
}

static void painter_draw_background(Scene* scene)
{
    Color bg = scene->palette.color[scene->bg_color];
    SDL_SetRenderDrawColor(renderer_, bg.r, bg.g, bg.b, SDL_ALPHA_OPAQUE);
    SDL_RenderFillRect(renderer_, &(SDL_Rect) {0, 0, GRAPHICS_W, GRAPHICS_H });
}

int painter_draw(Scene* scene)
{
    painter_draw_deep_background();
    painter_draw_background(scene);
    // painter_draw_text(scene.text_layer);

    SDL_RenderPresent(renderer_);

    return 0;
}

int painter_destroy()
{
    // TODO
    return 0;
}