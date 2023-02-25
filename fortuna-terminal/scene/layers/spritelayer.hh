#ifndef SPRITELAYER_HH_
#define SPRITELAYER_HH_

#include "imagelayer.hh"

struct SpriteState {
    size_t   pos_x = 0, pos_y = 0;
    bool     visible = false;
    bool     mirrored_h = false;
    bool     mirrored_v = false;
    uint8_t  z_order = 0;
    uint16_t image = 0;
};

class SpriteLayer : public ImageLayer {
public:
    static constexpr size_t MAX_SPRITE_IMAGES = 512;
    static constexpr size_t MAX_SPRITES = 128;

    SpriteState sprite_state[MAX_SPRITES] {};
};

#endif //SPRITELAYER_HH_
