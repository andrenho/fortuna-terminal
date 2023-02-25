#ifndef SPRITELAYER_HH_
#define SPRITELAYER_HH_

#include "imagelayer.hh"

class SpriteLayer : public ImageLayer {
public:
    static constexpr size_t MAX_SPRITES = 512;
};

#endif //SPRITELAYER_HH_
