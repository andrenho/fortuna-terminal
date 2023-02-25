#include "spritepainter.hh"
#include "exceptions/sdlexception.hh"

#include <cstring>
#include <iostream>
#include <iterator>

SpritePainter::SpritePainter(SDL_Renderer *renderer)
    : renderer_(renderer)
{
}

void SpritePainter::draw(SpriteLayer& sprite_layer)
{
    initialize_sprites(sprite_layer);

    struct PaintParameters {
        SDL_Texture*     tx = nullptr;
        size_t           z = 256 * SpriteLayer::MAX_SPRITES;
        SDL_RendererFlip flip = SDL_FLIP_NONE;
        SDL_Rect         dest { 0, 0, Image::IMAGE_W, Image::IMAGE_H };
    };
    PaintParameters pp[SpriteLayer::MAX_SPRITES];

    size_t pp_sz = 0;

    for (uint16_t i = 0; i < SpriteLayer::MAX_SPRITES; ++i) {
        SpriteState const& ss = sprite_layer.sprite_state[i];
        if (ss.visible) {
            pp[pp_sz].tx = textures_.at(ss.image).get();
            if (pp[pp_sz].tx == nullptr)
                break;
            pp[pp_sz].z = ss.z_order * SpriteLayer::MAX_SPRITES + i;

            if (ss.mirrored_h && !ss.mirrored_v)
                pp[pp_sz].flip = SDL_FLIP_HORIZONTAL;
            else if (!ss.mirrored_h && ss.mirrored_v)
                pp[pp_sz].flip = SDL_FLIP_VERTICAL;
            else if (ss.mirrored_h && ss.mirrored_v)
                pp[pp_sz].flip = static_cast<SDL_RendererFlip>(SDL_FLIP_VERTICAL | SDL_FLIP_HORIZONTAL);

            pp[pp_sz].dest.x = (int) ss.pos_x;
            pp[pp_sz].dest.y = (int) ss.pos_y;

            ++pp_sz;
        }
    }

    std::sort(std::begin(pp), std::begin(pp) + pp_sz,
              [](auto const& a, auto const& b) { return a.z < b.z; });

    for (size_t i = 0; i < pp_sz; ++i) {
        static SDL_Point center { 0, 0 };
        static SDL_Rect orig { 0, 0, Image::IMAGE_W, Image::IMAGE_H };
        SDL_RenderCopyEx(renderer_, pp[i].tx, &orig, &pp[i].dest, 0, &center, pp[i].flip);
    }
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

