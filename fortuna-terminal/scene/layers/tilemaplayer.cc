#include "tilemaplayer.hh"

#include "scene/scene.hh"

std::vector<ImageToDraw> TilemapLayer::images_to_draw(Scene const &scene) const
{
    std::vector<ImageToDraw> images_to_draw;

    Tilemap const& tilemap = scene.tilemap[map];
    for (int x = 0; x < (int) tilemap.w; ++x) {
        for (int y = 0; y < (int) tilemap.h; ++y) {
            size_t image = tilemap.images[x + (y * tilemap.w)];
            images_to_draw.push_back({
                (uint16_t) image,
                (int) (pos_x + (x * Image::IMAGE_W)),
                (int) (pos_y + (y * Image::IMAGE_H)),
                false, false
            });
        }
    }
    return images_to_draw;
}
