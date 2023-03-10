#include "textureatlas.hh"
#include "common/types/unique_ptr.hh"
#include "common/exceptions/sdlexception.hh"
#include "scene/scene.hh"
#include "terminal/sdl/sdl.hh"

#include <cstring>

void TextureAtlas::emplace_from_image(size_t index, size_t image_idx, Image const &image, Palette const& palette)
{
    SDL_Texture* scene_texture = get_or_create_texture(index);

    UniquePtrWithDeleter<SDL_Surface> sf = {
            SDL_CreateRGBSurface(0, Image::IMAGE_W, Image::IMAGE_H, 8, 0, 0, 0, 0),
            [](SDL_Surface* sf) { SDL_FreeSurface(sf); }
    };
    if (!sf)
        throw SDLException("Error creating surface");

    SDL_Color colors[PALETTE_SZ];
    for (size_t i = 0; i < PALETTE_SZ; ++i)
        colors[i] = { palette[i].r, palette[i].g, palette[i].b, SDL_ALPHA_OPAQUE };
    SDL_SetPaletteColors(sf->format->palette, colors, 0, PALETTE_SZ);

    if (image.transparent_color < PALETTE_SZ)
        SDL_SetColorKey(sf.get(), SDL_RLEACCEL, image.transparent_color);

    memcpy(sf->pixels, image.pixels, Image::IMAGE_SZ);
    UniquePtrWithDeleter<SDL_Texture> texture = {
            SDL_CreateTextureFromSurface(renderer_, sf.get()),
            [](SDL_Texture* tx) { SDL_DestroyTexture(tx); }
    };
    if (!texture)
        throw SDLException("Error creating texture");

    SDL_SetTextureBlendMode(texture.get(), SDL_BLENDMODE_BLEND);
    if (SDL_SetRenderTarget(renderer_, scene_texture) < 0)
        throw SDLException("Error setting target");
    auto [x, y] = index_location_in_texture(image_idx);
    SDL_Rect src = { 0, 0, Image::IMAGE_W, Image::IMAGE_H };
    SDL_Rect dest = { x, y, Image::IMAGE_W, Image::IMAGE_H };
    if (SDL_RenderCopy(renderer_, texture.get(), &src, &dest) < 0)
        throw SDLException("Error rendering target");
    SDL_SetRenderTarget(renderer_, nullptr);
}

SDL_Texture *TextureAtlas::get_or_create_texture(size_t scene_id)
{
    auto it = textures_.find(scene_id);

    if (it != textures_.end()) {
        return it->second.get();
    } else {
        int h = (int) (Scene::MAX_IMAGES / (MAX_TEXTURE_W / Image::IMAGE_W) + 1) * 16;
        UniquePtrTexture texture = SDL::create_independent_texture(renderer_, SDL_PIXELFORMAT_ARGB8888,
                                                                   SDL_TEXTUREACCESS_TARGET, MAX_TEXTURE_W, h);
        SDL_Texture* tx_ptr = texture.get();
        SDL_SetTextureBlendMode(tx_ptr, SDL_BLENDMODE_BLEND);
        textures_.emplace(scene_id, std::move(texture));
        return tx_ptr;
    }
}

TextureInfo TextureAtlas::get_texture(size_t index, size_t image_key) const
{
    auto [x, y] = index_location_in_texture(image_key);
    return {
        textures_.at(index).get(),
        { x, y, Image::IMAGE_W, Image::IMAGE_H },
    };
}

std::pair<int, int> TextureAtlas::index_location_in_texture(size_t key)
{
    int x = ((int) key * 16) % MAX_TEXTURE_W;
    int y = ((int) key * 16) / MAX_TEXTURE_W;
    return { x, y };
}

void TextureAtlas::reset(size_t scene_id)
{
    textures_.erase(scene_id);
}
