#ifndef TILEMAPPAINTER_HH_
#define TILEMAPPAINTER_HH_

#include <SDL2/SDL.h>
#include "imagepainter.hh"

class TilemapPainter : public ImagePainter {
public:
    explicit TilemapPainter(SDL_Renderer* renderer) : ImagePainter(renderer) {}

private:
    [[nodiscard]] std::vector<ImageToPaint> images_to_paint(size_t texture_idx, ImageLayer const &layer, TextureManager const& texture_manager) const override;
};

#endif //TILEMAPPAINTER_HH_
