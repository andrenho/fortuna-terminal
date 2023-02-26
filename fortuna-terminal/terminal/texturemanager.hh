#ifndef TEXTUREMANAGER_HH_
#define TEXTUREMANAGER_HH_

#include <cstddef>

#include <functional>
#include <memory>
#include <vector>

#include <SDL2/SDL.h>

#include "scene/layers/imagelayer.hh"

struct TextureInfo {
    SDL_Texture* tx;
    SDL_Rect     src;
};

class TextureManager {
public:
    TextureManager(SDL_Renderer* renderer, size_t textures, size_t images_per_texture);

    void        add_image(size_t texture_n, Image const& image, Palette const& palette);
    TextureInfo texture_info(size_t texture_n, size_t image_key) const;

private:
    using TextureUniquePtr = std::unique_ptr<SDL_Texture, std::function<void(SDL_Texture*)>>;
    std::vector<TextureUniquePtr> textures_ {};

    static std::pair<int, int> index_location_in_texture(size_t key);

    SDL_Renderer* renderer_;

    static constexpr size_t MAX_TEXTURE_W = 2048;   // these are the maximums in Raspberry Pi Zero W
    static constexpr size_t MAX_TEXTURE_H = 2048;
};

#endif //TEXTUREMANAGER_HH_
