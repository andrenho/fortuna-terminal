#ifndef SPRITEPAINTER_HH_
#define SPRITEPAINTER_HH_

#include <SDL2/SDL.h>
#include "../../common/noncopyable.hh"
#include "scene/layers/spritelayer.hh"

#include <functional>
#include <memory>
#include <unordered_map>

class SpritePainter : NonCopyable {
public:
    explicit SpritePainter(SDL_Renderer* renderer);

    void draw(SpriteLayer& sprite_layer);

private:
    SDL_Renderer* renderer_;

    void initialize_sprites(SpriteLayer& layer);

    using TextureUniquePtr = std::unique_ptr<SDL_Texture, std::function<void(SDL_Texture*)>>;
    std::unordered_map<uint16_t, TextureUniquePtr> textures_ {};
};

#endif //SPRITEPAINTER_HH_
