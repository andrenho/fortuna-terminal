#ifndef IMAGEPAINTER_HH_
#define IMAGEPAINTER_HH_

#include <SDL2/SDL.h>
#include "../../common/noncopyable.hh"
#include "scene/layers/layer.hh"
#include "terminal/texturemanager.hh"

class ImagePainter : NonCopyable {
public:
    void draw(size_t texture_idx, ImageLayer& layer, TextureManager& texture_manager);

protected:
    struct ImageToPaint : NonCopyable {
        SDL_Texture*     tx = nullptr;
        size_t           z = 0;
        SDL_RendererFlip flip = SDL_FLIP_NONE;
        SDL_Rect         src { 0, 0, 0, 0 };
        SDL_Rect         dest { 0, 0, 0, 0 };
    };

    explicit ImagePainter(SDL_Renderer* renderer) : renderer_(renderer) {}

    [[nodiscard]] virtual std::vector<ImageToPaint> images_to_paint(size_t texture_idx, ImageLayer const& layer, TextureManager const& texture_manager) const = 0;

private:
    SDL_Renderer* renderer_;

    static void initialize_sprites(size_t texture_idx, ImageLayer &layer, TextureManager &manager);
};

#endif //IMAGEPAINTER_HH_
