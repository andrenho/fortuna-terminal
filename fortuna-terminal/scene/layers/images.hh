#ifndef IMAGES_HH_
#define IMAGES_HH_

#include "layer.hh"
#include "common/syncqueue.hh"

struct Image {
    static constexpr int IMAGE_W = 16;
    static constexpr int IMAGE_H = 16;
    static constexpr int IMAGE_SZ = IMAGE_W * IMAGE_H;

    uint8_t  transparent_color;
    uint8_t  pixels[IMAGE_W * IMAGE_H];
};

#endif //IMAGES_HH_
