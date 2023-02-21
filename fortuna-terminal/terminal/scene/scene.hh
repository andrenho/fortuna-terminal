#ifndef SCENE_HH_
#define SCENE_HH_

#include "layers/text.hh"
#include "../../common/noncopyable.hh"
#include "common/geometry.hh"

struct Scene {
    Scene();

    void set_graphical_mode(GraphicalMode graphical_mode);

    Text text;

    [[nodiscard]] Size terminal_size() const;

private:
    GraphicalMode graphical_mode_ = TEXT_80_COLUMNS;
};

#endif //SCENE_HH_
