#ifndef SPRITELAYER_HH_
#define SPRITELAYER_HH_

#include "imagelayer.hh"

struct Sprite {
    int      pos_x = 0, pos_y = 0;
    bool     visible = false;
    bool     mirrored_h = false;
    bool     mirrored_v = false;
    uint8_t  z_order = 0;
    uint16_t image = 0;
};

class SpriteLayer : public ImageLayer {
public:
    static constexpr size_t MAX_SPRITES = 128;
    Sprite sprites[MAX_SPRITES] {};

    [[nodiscard]] std::vector<ImageToDraw> images_to_draw([[maybe_unused]] class Scene const& scene) const override;
};

#endif //SPRITELAYER_HH_
