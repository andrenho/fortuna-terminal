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

}

