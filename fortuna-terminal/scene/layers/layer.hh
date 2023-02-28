#ifndef LAYER_HH_
#define LAYER_HH_

#include <mutex>
#include <thread>

#include "../palette/palette.hh"
#include "common/noncopyable.hh"

class Layer {
public:
    virtual ~Layer() = default;

    bool enabled = true;
};

#endif //LAYER_HH_
