#include "font.h"
#include "painter.h"
#include "error/error.h"
#include "ui.h"

#include <stdio.h>

#define TEXT_CHAR_W 6
#define TEXT_CHAR_H 9

static SDL_Renderer* renderer_ = NULL;
static SDL_Texture* font_ = NULL;

static FT_Result painter_load_font()
{
    SDL_RWops* io = SDL_RWFromConstMem(font_bmp, (int) font_bmp_len);
    SDL_Surface* sf = SDL_LoadBMP_RW(io, 1);
    if (!sf)
        return FT_ERR_SDL;

    SDL_SetColorKey(sf, SDL_RLEACCEL, 0);
    font_ = SDL_CreateTextureFromSurface(renderer_, sf);
    if (!font_)
        return FT_ERR_SDL;

    SDL_FreeSurface(sf);

    return 0;
}

FT_Result painter_init(SDL_Window *window)
{

    renderer_ = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
    if (!renderer_)
        return FT_ERR_SDL;
    SDL_RenderSetLogicalSize(renderer_, GRAPHICS_W, GRAPHICS_H);

    if (options.debug_mode) {
        SDL_RendererInfo info;
        SDL_GetRendererInfo(renderer_, &info );
        printf("SDL_RENDER_DRIVER selected: %s\n", info.name);
    }

    E_CHECK(painter_load_font(), "Error loading font");

    return FT_OK;
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

static void painter_draw_text_cell(Text* text, unsigned int line, unsigned int column, Color* bg)
{
    Char chr = text_get(text, line, column);
    uint8_t c = chr.c;

    unsigned int offset_x = (GRAPHICS_W / 2) - (TEXT_CHAR_W * text->columns / 2);
    unsigned int offset_y = (GRAPHICS_H / 2) - (TEXT_CHAR_H * text->lines / 2);
    unsigned int orig_x = (c / 16) * TEXT_CHAR_W;
    unsigned int orig_y = (c % 16) * TEXT_CHAR_H;
    unsigned int dest_x = (column * TEXT_CHAR_W) + offset_x;
    unsigned int dest_y = (line * TEXT_CHAR_H) + offset_y;

    Color* fg = &text->palette.color[chr.a.color];

    if (text->cursor.x == column && text->cursor.y == line && text->cursor.visible && text->cursor.blink_state) {
        Color cg = text->palette.color[text->cursor.color];
        SDL_SetRenderDrawColor(renderer_, cg.r, cg.g, cg.b, SDL_ALPHA_OPAQUE);
        SDL_Rect r = { (int) dest_x, (int) dest_y, TEXT_CHAR_W, TEXT_CHAR_H };
        SDL_RenderFillRect(renderer_, &r);

        SDL_SetTextureColorMod(font_, bg->r, bg->g, bg->b);

    } else if (chr.a.reverse) {
        SDL_SetRenderDrawColor(renderer_, fg->r, fg->g, fg->b, SDL_ALPHA_OPAQUE);
        SDL_Rect r = { (int) dest_x, (int) dest_y, TEXT_CHAR_W, TEXT_CHAR_H };
        SDL_RenderFillRect(renderer_, &r);

        SDL_SetTextureColorMod(font_, bg->r, bg->g, bg->b);

    } else {
        SDL_SetTextureColorMod(font_, fg->r, fg->g, fg->b);
    }

    if (c != 0 && c != 32) {
        SDL_RenderCopy(renderer_, font_,
                       &(SDL_Rect) { (int) orig_x, (int) orig_y, TEXT_CHAR_W, TEXT_CHAR_H },
                       &(SDL_Rect) { (int) dest_x, (int) dest_y, TEXT_CHAR_W, TEXT_CHAR_H });
    }
}

static void painter_draw_text(Text* text, Color* bg)
{
    for (size_t line = 0; line < text->lines; ++line)
        for (size_t column = 0; column < text->columns; ++column)
            painter_draw_text_cell(text, line, column, bg);
}

void painter_draw(Scene* scene)
{
    painter_draw_deep_background();
    painter_draw_background(scene);

    Color* bg = &scene->palette.color[scene->bg_color];
    painter_draw_text(&scene->text, bg);

    SDL_RenderPresent(renderer_);
}

void painter_destroy()
{
    if (font_)
        SDL_DestroyTexture(font_);
    if (renderer_)
        SDL_DestroyRenderer(renderer_);
}