#ifndef SCENE_HH_
#define SCENE_HH_

#include "palette.hh"
#include "textlayer.hh"
#include "event/inputevent.hh"

struct Scene {
    TextLayer text_layer;

    void update(InputQueue& input_queue);
};

#endif //SCENE_HH_
