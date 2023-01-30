#ifndef LAYER_HH_
#define LAYER_HH_

#include "palette.hh"

class Layer {
public:
    Color palette_color(uint8_t idx) { return palette_.color[idx % Palette::SIZE]; }

protected:
    Palette palette_;
};

#endif //LAYER_HH_
