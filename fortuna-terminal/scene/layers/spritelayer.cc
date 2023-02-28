#include "spritelayer.hh"

std::vector<ImageToDraw> SpriteLayer::images_to_draw(class Scene const& scene) const
{
    std::vector<ImageToDraw> images_to_draw;
    images_to_draw.reserve(MAX_SPRITES);

    Sprite sprite_copy[MAX_SPRITES] {};
    std::copy_if(std::begin(sprites), std::end(sprites), sprite_copy, [](Sprite const& s) { return s.visible; });
    std::sort(std::begin(sprite_copy), std::end(sprite_copy),
              [](auto const& a, auto const& b) { return a.z_order < b.z_order; });

    for (auto const& s : sprite_copy) {
        if (s.visible)
            images_to_draw.push_back({ s.image, s.pos_x, s.pos_y, s.mirrored_h, s.mirrored_v });
    }

    return images_to_draw;
}
