#ifndef LAYER_HH_
#define LAYER_HH_

#include "terminal/palette/palette.hh"

class Layer {
protected:
    Layer() { palette_init(palette); }

public:
    Palette palette = {};
};

#endif //LAYER_HH_
