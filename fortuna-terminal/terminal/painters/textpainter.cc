#include "textpainter.hh"

#include "terminal/painters/data/font.h"
#include "common/exceptions/sdlexception.hh"

TextPainter::TextPainter(SDL_Renderer *renderer)
    : renderer_(renderer)
{
    SDL_RWops* io = SDL_RWFromConstMem(font_bmp, (int) font_bmp_len);
    UniquePtrWithDeleter<SDL_Surface> sf {
            SDL_LoadBMP_RW(io, 1),
            [](SDL_Surface* sf) { SDL_FreeSurface(sf); }
    };
    if (!sf)
        throw SDLException("Error loading font BMP");

    SDL_SetColorKey(sf.get(), SDL_RLEACCEL, 0);
    font_ = {
            SDL_CreateTextureFromSurface(renderer_, sf.get()),
            [](SDL_Texture* t) { SDL_DestroyTexture(t); }
    };
    if (!font_)
        throw SDLException("Could not create texture for font");
}

void TextPainter::draw(Scene const& scene) const
{
    TextLayer const& text = scene.text();
    if (!text.enabled)
        return;

    for (size_t y = 0; y < text.lines(); ++y)
        for (size_t x = 0; x < text.columns(); ++x)
            draw_cell(text, y, x, scene.palette, scene.bg_color);
}

void TextPainter::draw_cell(TextLayer const &text, size_t line, size_t column, Palette const palette, uint8_t bg_color) const
{
    Char chr = text.get_char(line, column);
    uint8_t c = chr.c;

    if (c == ' ' && !(text.cursor().x == column && text.cursor().y == line) && !chr.attrib.reverse)
        return;

    unsigned int orig_x = (c / 16) * TextChar_W;
    unsigned int orig_y = (c % 16) * TextChar_H;
    unsigned int dest_x = (unsigned int) (column * TextChar_W);
    unsigned int dest_y = (unsigned int) (line * TextChar_H);
    if (text.mode() == Mode::Text) {
        dest_x += TextBorder;
        dest_y += TextBorder;
    }

    Color fg = palette[chr.attrib.color];
    Color bg = palette[bg_color];

    SDL_Rect r_orig = {(int) orig_x, (int) orig_y, TextChar_W, TextChar_H };
    SDL_Rect r_dest = {(int) dest_x, (int) dest_y, TextChar_W, TextChar_H };

    if (text.cursor().x == column && text.cursor().y == line && text.cursor().attrib.visible && text.cursor().blink_state) {
        // character inverted due to cursor
        Color ch_color = palette[text.cursor().attrib.color];
        SDL_SetRenderDrawColor(renderer_, ch_color.r, ch_color.g, ch_color.b, SDL_ALPHA_OPAQUE);
        SDL_Rect r = { (int) dest_x, (int) dest_y, TextChar_W, TextChar_H };
        SDL_RenderFillRect(renderer_, &r);

        SDL_SetTextureColorMod(font_.get(), bg.r, bg.g, bg.b);

    } else if (chr.attrib.reverse) {
        // reverse character
        SDL_SetRenderDrawColor(renderer_, fg.r, fg.g, fg.b, SDL_ALPHA_OPAQUE);
        SDL_Rect r = { (int) dest_x, (int) dest_y, TextChar_W, TextChar_H };
        SDL_RenderFillRect(renderer_, &r);

        SDL_SetTextureColorMod(font_.get(), bg.r, bg.g, bg.b);

    } else {
        // regular character
        SDL_SetTextureColorMod(font_.get(), fg.r, fg.g, fg.b);
    }

    if (c != 0 && c != 32)
        SDL_RenderCopy(renderer_, font_.get(), &r_orig, &r_dest);
}
