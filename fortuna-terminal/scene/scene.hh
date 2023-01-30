#ifndef SCENE_HH_
#define SCENE_HH_

#include "palette.hh"
#include "textlayer.hh"

struct Scene {
    TextLayer text_layer;

    void update();
};

#endif //SCENE_HH_
