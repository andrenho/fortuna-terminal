#ifndef LAYER_HH_
#define LAYER_HH_

#include <mutex>
#include <thread>

#include "../palette/palette.hh"

class Layer {
public:
    Palette palette = {};

protected:
    Layer() { palette_init(palette); }
};

#endif //LAYER_HH_
