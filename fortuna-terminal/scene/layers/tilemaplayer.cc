#include "tilemaplayer.hh"

#include "scene/scene.hh"
#include "scene/tilemap.hh"
#include "images.hh"

std::vector<ImageToDraw> TilemapLayer::images_to_draw(Scene const &scene) const
{
    std::vector<ImageToDraw> images_to_draw;

    if (map < 0)
        return {};

    Tilemap const& tilemap = scene.tilemaps[map];
    for (int x = 0; x < (int) tilemap.w; ++x) {
        for (int y = 0; y < (int) tilemap.h; ++y) {
            ssize_t image = tilemap.image_indexes[x + (y * tilemap.w)];
            if (image >= 0) {
                images_to_draw.push_back({
                    (uint16_t) image,
                    (int) (pos_x + (x * Image::IMAGE_W)),
                    (int) (pos_y + (y * Image::IMAGE_H)),
                    false, false
                });
            }
        }
    }
    return images_to_draw;
}

void TilemapLayer::reset()
{
    pos_x = 0;
    pos_y = 0;
    map = -1;
}
