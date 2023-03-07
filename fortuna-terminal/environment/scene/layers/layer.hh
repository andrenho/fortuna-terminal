#ifndef LAYER_HH_
#define LAYER_HH_

#include <mutex>
#include <thread>

#include "old/scene/palette/palette.hh"
#include "old/common/noncopyable.hh"

class Layer {
public:
    virtual ~Layer() = default;

    virtual void reset() = 0;

    bool enabled = true;
};

#endif //LAYER_HH_
