#include "texturemanager.hh"
#include "common/exceptions/sdlexception.hh"
#include "scene/scene.hh"

#include <cstring>

void TextureManager::add_image(size_t scene_id, Image const &image, Palette const& palette)
{
    SDL_Texture* scene_texture = get_or_create_texture(scene_id);

    SDL_Surface* sf = SDL_CreateRGBSurface(0, Image::IMAGE_W, Image::IMAGE_H, 8, 0, 0, 0, 0);
    if (!sf)
        throw SDLException("Error creating surface");

    SDL_Color colors[PALETTE_SZ];
    for (size_t i = 0; i < PALETTE_SZ; ++i)
        colors[i] = { palette[i].r, palette[i].g, palette[i].b, SDL_ALPHA_OPAQUE };
    SDL_SetPaletteColors(sf->format->palette, colors, 0, PALETTE_SZ);

    if (image.transparent_color < PALETTE_SZ)
        SDL_SetColorKey(sf, SDL_RLEACCEL, image.transparent_color);

    memcpy(sf->pixels, image.pixels, Image::IMAGE_SZ);
    SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer_, sf);
    if (texture == nullptr)
        throw SDLException("Error creating texture");

    SDL_SetTextureBlendMode(texture, SDL_BLENDMODE_BLEND);
    if (SDL_SetRenderTarget(renderer_, scene_texture) < 0)
        throw SDLException("Error setting target");
    auto [x, y] = index_location_in_texture(image.index);
    SDL_Rect src = { 0, 0, Image::IMAGE_W, Image::IMAGE_H };
    SDL_Rect dest = { x, y, Image::IMAGE_W, Image::IMAGE_H };
    if (SDL_RenderCopy(renderer_, texture, &src, &dest) < 0)
        throw SDLException("Error rendering target");
    SDL_SetRenderTarget(renderer_, nullptr);

    SDL_DestroyTexture(texture);
    SDL_FreeSurface(sf);
}

SDL_Texture *TextureManager::get_or_create_texture(size_t scene_id)
{
    auto it = textures_.find(scene_id);

    if (it != textures_.end()) {
        return it->second.get();
    } else {
        int h = (int) (Scene::MAX_IMAGES / (MAX_TEXTURE_W / Image::IMAGE_W) + 1) * 16;
        auto [itr, _] = textures_.emplace(
                scene_id,
                TextureUniquePtr(
                        SDL_CreateTexture(renderer_, SDL_PIXELFORMAT_ARGB8888, SDL_TEXTUREACCESS_TARGET, MAX_TEXTURE_W, h),
                        [](SDL_Texture* t) { SDL_DestroyTexture(t); }
                )
        );
        SDL_Texture* texture = itr->second.get();
        if (!texture)
            throw SDLException("Error creating texture");
        SDL_SetTextureBlendMode(texture, SDL_BLENDMODE_BLEND);
        return texture;
    }
}

TextureInfo TextureManager::texture_info(size_t texture_n, size_t image_key) const
{
    auto [x, y] = index_location_in_texture(image_key);
    return {
            textures_.at(texture_n).get(),
        { x, y, Image::IMAGE_W, Image::IMAGE_H },
    };
}

std::pair<int, int> TextureManager::index_location_in_texture(size_t key)
{
    int x = ((int) key * 16) % MAX_TEXTURE_W;
    int y = ((int) key * 16) / MAX_TEXTURE_W;
    return { x, y };
}