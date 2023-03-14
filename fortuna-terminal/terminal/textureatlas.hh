#ifndef TEXTUREATLAS_HH_
#define TEXTUREATLAS_HH_

#include <cstddef>

#include <functional>
#include <memory>
#include <unordered_map>

#include "lib/SDL2-windows/include/SDL2/SDL.h"

#include "scene/layers/images.hh"
#include "scene/palette/palette.hh"

struct TextureInfo {
    SDL_Texture* tx;
    SDL_Rect     src;
};

class TextureAtlas {
public:
    explicit TextureAtlas(SDL_Renderer* renderer) : renderer_(renderer) {}

    void        emplace_from_image(size_t index, Image const& image, Palette const& palette);
    TextureInfo get_texture(size_t index, size_t image_key) const;

private:
    SDL_Renderer* renderer_;
    std::unordered_map<size_t, SDL_Texture*> textures_ {};

    SDL_Texture* get_or_create_texture(size_t scene_id);

    static constexpr size_t MAX_TEXTURE_W = 2048;   // these are the maximums in Raspberry Pi Zero W
    static constexpr size_t MAX_TEXTURE_H = 2048;

    static std::pair<int, int> index_location_in_texture(size_t key);
};

#endif //TEXTUREATLAS_HH_
