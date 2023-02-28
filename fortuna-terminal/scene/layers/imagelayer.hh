#ifndef IMAGELAYER_HH_
#define IMAGELAYER_HH_

#include <vector>

#include "layer.hh"

struct ImageToDraw {
    uint16_t image = 0;
    int      pos_x = 0, pos_y = 0;
    bool     mirrored_h = false;
    bool     mirrored_v = false;
};

struct ImageLayer : public Layer {
    virtual std::vector<ImageToDraw> images_to_draw() const = 0;
};

#endif //IMAGELAYER_HH_
