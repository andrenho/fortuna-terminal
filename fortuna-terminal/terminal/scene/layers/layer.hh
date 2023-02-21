#ifndef LAYER_HH_
#define LAYER_HH_

#include <thread>

#include "terminal/palette/palette.hh"

class Layer {
public:
    Palette palette = {};
    mutable std::unique_ptr<std::mutex> mutex_ = std::make_unique<std::mutex>();

protected:
    Layer() { palette_init(palette); }
};

#endif //LAYER_HH_
