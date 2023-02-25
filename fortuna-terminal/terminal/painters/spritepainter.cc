#include "spritepainter.hh"
#include "exceptions/sdlexception.hh"

#include <cstring>

SpritePainter::SpritePainter(SDL_Renderer *renderer)
    : renderer_(renderer)
{
}

void SpritePainter::draw(SpriteLayer& sprite_layer)
{
    initialize_sprites(sprite_layer);
}

void SpritePainter::initialize_sprites(SpriteLayer& layer)
{
    std::optional<Image> oimg;
    while ((oimg = layer.pending_images()->pop_nonblock()).has_value()) {
        Image const& img = oimg.value();

        SDL_Surface* sf = SDL_CreateRGBSurface(0, Image::IMAGE_W, Image::IMAGE_H, 8, 0, 0, 0, 0);
        if (!sf)
            throw SDLException("Error creating surface");

        SDL_Color colors[PALETTE_SZ];
        for (size_t i = 0; i < PALETTE_SZ; ++i)
            colors[i] = { layer.palette[i].r, layer.palette[i].g, layer.palette[i].b, SDL_ALPHA_OPAQUE };
        SDL_SetPaletteColors(sf->format->palette, colors, 0, PALETTE_SZ);

        if (img.transparent_color < PALETTE_SZ)
            SDL_SetColorKey(sf, SDL_RLEACCEL, img.transparent_color);

        memcpy(sf->pixels, img.image, Image::IMAGE_SZ);
        textures_.emplace(img.key, TextureUniquePtr(
                SDL_CreateTextureFromSurface(renderer_, sf),
                [](SDL_Texture* t) { SDL_DestroyTexture(t); }
        ));
        if (textures_.at(img.key) == nullptr)
            throw SDLException("Error creating texture");
        SDL_FreeSurface(sf);
    }
}

