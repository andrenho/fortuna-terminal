#ifndef SPRITEPAINTER_HH_
#define SPRITEPAINTER_HH_

#include <SDL2/SDL.h>
#include "../../common/noncopyable.hh"
#include "scene/layers/spritelayer.hh"
#include "terminal/texturemanager.hh"

#include <functional>
#include <memory>
#include <unordered_map>

class SpritePainter : NonCopyable {
public:
    explicit SpritePainter(SDL_Renderer* renderer) : renderer_(renderer) {}

    void draw(SpriteLayer& sprite_layer, TextureManager& texture_manager);

private:
    SDL_Renderer* renderer_;

    void initialize_sprites(SpriteLayer &layer, TextureManager &manager);
};

#endif //SPRITEPAINTER_HH_
