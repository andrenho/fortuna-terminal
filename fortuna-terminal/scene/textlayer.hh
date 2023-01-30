#ifndef TEXTLAYER_HH_
#define TEXTLAYER_HH_

#include <cstdint>
#include "palette.hh"
#include "layer.hh"

class TextLayer : public Layer {
public:
    uint8_t background_color = Color::BLACK;
};

#endif //TEXTLAYER_HH_
