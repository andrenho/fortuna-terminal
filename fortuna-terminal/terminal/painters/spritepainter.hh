#ifndef SPRITEPAINTER_HH_
#define SPRITEPAINTER_HH_

#include <SDL2/SDL.h>
#include "terminal/texturemanager.hh"
#include "imagepainter.hh"

#include <functional>
#include <memory>
#include <unordered_map>

class SpritePainter : public ImagePainter {
public:
    explicit SpritePainter(SDL_Renderer* renderer) : ImagePainter(renderer) {}

private:
    [[nodiscard]] std::vector<ImageToPaint> images_to_paint(Scene const& scene, ImageLayer const &layer, TextureManager const& texture_manager) const override;
};

#endif //SPRITEPAINTER_HH_
