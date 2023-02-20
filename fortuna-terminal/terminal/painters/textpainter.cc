#include "textpainter.hh"

#include "font/font.h"
#include "exceptions/sdlexception.hh"

TextPainter::TextPainter(SDL_Renderer *renderer)
    : renderer_(renderer)
{
    SDL_RWops* io = SDL_RWFromConstMem(font_bmp, (int) font_bmp_len);
    SDL_Surface* sf = SDL_LoadBMP_RW(io, 1);
    if (!sf)
        throw SDLException("Error loading font BMP");

    SDL_SetColorKey(sf, SDL_RLEACCEL, 0);
    font_ = decltype(font_)(
            SDL_CreateTextureFromSurface(renderer_, sf),
            [](SDL_Texture* t) { SDL_DestroyTexture(t); }
    );
    if (!font_)
        throw SDLException("Could not create texture for font");

    SDL_FreeSurface(sf);
}

void TextPainter::draw(Text const &text) const
{
    for (size_t y = 0; y < text.lines(); ++y)
        for (size_t x = 0; x < text.columns(); ++x)
            draw_cell(text, y, x);
}

void TextPainter::draw_cell(Text const &text, size_t line, size_t column) const
{
    Char chr = text.get(line, column);
    uint8_t c = chr.c;

    unsigned int orig_x = (c / 16) * TextChar_W;
    unsigned int orig_y = (c % 16) * TextChar_H;
    unsigned int dest_x = (column * TextChar_W) + TextBorder;
    unsigned int dest_y = (line * TextChar_H) + TextBorder;

    Color fg = text.palette[chr.attrib.color];
    Color bg = text.palette[text.bg_color];

    if (text.cursor().x == column && text.cursor().y == line && text.cursor().attrib.visible && text.cursor().blink_state) {
        Color cg = text.palette[text.cursor().attrib.color];
        SDL_SetRenderDrawColor(renderer_, cg.r, cg.g, cg.b, SDL_ALPHA_OPAQUE);
        SDL_Rect r = { (int) dest_x, (int) dest_y, TextChar_W, TextChar_H };
        SDL_RenderFillRect(renderer_, &r);

        SDL_SetTextureColorMod(font_.get(), bg.r, bg.g, bg.b);

    } else if (chr.attrib.reverse) {
        SDL_SetRenderDrawColor(renderer_, fg.r, fg.g, fg.b, SDL_ALPHA_OPAQUE);
        SDL_Rect r = { (int) dest_x, (int) dest_y, TextChar_W, TextChar_H };
        SDL_RenderFillRect(renderer_, &r);

        SDL_SetTextureColorMod(font_.get(), bg.r, bg.g, bg.b);

    } else {
        SDL_SetTextureColorMod(font_.get(), fg.r, fg.g, fg.b);
    }

    if (c != 0 && c != 32) {
        SDL_Rect r1 = { (int) orig_x, (int) orig_y, TextChar_W, TextChar_H };
        SDL_Rect r2 = { (int) dest_x, (int) dest_y, TextChar_W, TextChar_H };
        SDL_RenderCopy(renderer_, font_.get(), &r1, &r2);
    }
}

void TextPainter::draw_background(Text const &text) const
{
    Color bg = text.palette[text.bg_color];
    SDL_SetRenderDrawColor(renderer_, bg.r, bg.g, bg.b, SDL_ALPHA_OPAQUE);
    SDL_Rect r = {
            0, 0,
            (int) ((text.columns() * TextChar_W) + (2 * TextBorder)),
            (int) ((text.lines() * TextChar_H) + (2 * TextBorder)),
    };
    SDL_RenderFillRect(renderer_, &r);
}

