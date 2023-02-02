#include "sdl_painter.hh"
#include "sdl_terminal.hh"
#include "debugmode.hh"

#include <SDL2/SDL.h>

#include "font.hh"

void SDL_Painter::initialize(struct SDL_Window* window)
{
    renderer_ = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
    if (!renderer_) {
        std::cerr << "SDL_CreateRenderer(): " << SDL_GetError() << "\n";
        exit(EXIT_FAILURE);
    }
    SDL_RenderSetLogicalSize(renderer_, SDL_Terminal::GRAPHICS_W, SDL_Terminal::GRAPHICS_H);

    if (debug_mode) {
        SDL_RendererInfo info;
        SDL_GetRendererInfo(renderer_, &info );
        std::cout << "SDL_RENDER_DRIVER selected: " << info.name << "\n";
    }

    load_font();
}

void SDL_Painter::load_font()
{
    SDL_RWops* io = SDL_RWFromConstMem(font_bmp, (int) font_bmp_len);
    SDL_Surface* sf = SDL_LoadBMP_RW(io, 1);
    if (!sf) {
        fprintf(stderr, "SDL_LoadBMP_RW: %s\n", SDL_GetError());
    }

    SDL_SetColorKey(sf, SDL_RLEACCEL, 0);
    font_ = SDL_CreateTextureFromSurface(renderer_, sf);

    SDL_FreeSurface(sf);
}

void SDL_Painter::finalize()
{
    if (font_)
        SDL_DestroyTexture(font_);
    if (renderer_)
        SDL_DestroyRenderer(renderer_);
}

void SDL_Painter::draw_deep_background() const
{
    SDL_SetRenderDrawColor(renderer_, 0, 0, 0, SDL_ALPHA_OPAQUE);
    SDL_RenderClear(renderer_);
}

void SDL_Painter::draw_background(const TextLayer &text_layer) const
{
    Color bg = text_layer.palette_color(text_layer.background_color());
    SDL_SetRenderDrawColor(renderer_, bg.r, bg.g, bg.b, SDL_ALPHA_OPAQUE);
    SDL_Rect r = {0, 0, SDL_Terminal::GRAPHICS_W, SDL_Terminal::GRAPHICS_H };
    SDL_RenderFillRect(renderer_, &r);
}

void SDL_Painter::draw_text(const TextLayer &text_layer) const
{
    /*
    SDL_Color fg = palette_color(COLOR_WHITE);
    SDL_SetTextureColorMod(font, fg.r, fg.g, fg.b);
    */

    for (size_t line = 0; line < text_layer.lines(); ++line)
        for (size_t column = 0; column < text_layer.columns(); ++column)
            draw_text_cell(text_layer, line, column);
}

void SDL_Painter::draw_text_cell(TextLayer const& text_layer, uint16_t line, uint16_t column) const
{
    Char chr = text_layer.chr(line, column);
    uint8_t c = chr.chr;

    unsigned int offset_x = (SDL_Terminal::GRAPHICS_W / 2) - (TEXT_CHAR_W * text_layer.columns() / 2);
    unsigned int offset_y = (SDL_Terminal::GRAPHICS_H / 2) - (TEXT_CHAR_H * text_layer.lines() / 2);
    unsigned int orig_x = (c / 16) * TEXT_CHAR_W;
    unsigned int orig_y = (c % 16) * TEXT_CHAR_H;
    unsigned int dest_x = (column * TEXT_CHAR_W) + offset_x;
    unsigned int dest_y = (line * TEXT_CHAR_H) + offset_y;

    Cursor const& cursor = text_layer.cursor();

    if (cursor.x == column && cursor.y == line && cursor.visible && cursor.blink_state) {
        Color cg = text_layer.palette_color(cursor.color);
        SDL_SetRenderDrawColor(renderer_, cg.r, cg.g, cg.b, SDL_ALPHA_OPAQUE);
        SDL_Rect r = { (int) dest_x, (int) dest_y, TEXT_CHAR_W, TEXT_CHAR_H };
        SDL_RenderFillRect(renderer_, &r);

        Color bg = text_layer.palette_color(text_layer.background_color());
        SDL_SetTextureColorMod(font_, bg.r, bg.g, bg.b);

    } else {
        Color fg = text_layer.palette_color(chr.color);
        SDL_SetTextureColorMod(font_, fg.r, fg.g, fg.b);
    }

    if (c != 0 && c != 32) {
        SDL_Rect src = { (int) orig_x, (int) orig_y, TEXT_CHAR_W, TEXT_CHAR_H };
        SDL_Rect dest = { (int) dest_x, (int) dest_y, TEXT_CHAR_W, TEXT_CHAR_H };
        SDL_RenderCopy(renderer_, font_, &src, &dest);
    }
}

void SDL_Painter::present() const
{
    SDL_RenderPresent(renderer_);
}