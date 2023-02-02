#ifndef SCENE_HH_
#define SCENE_HH_

#include "palette.hh"
#include "textlayer.hh"
#include "event/inputevent.hh"

struct Scene {
    TextLayer text_layer;

    void process_input_queue();
};

#endif //SCENE_HH_
