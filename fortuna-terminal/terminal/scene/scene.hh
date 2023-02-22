#ifndef SCENE_HH_
#define SCENE_HH_

#include "layers/text.hh"
#include "../../common/noncopyable.hh"
#include "common/geometry.hh"

struct Scene {
    Scene();

    void set_graphical_mode(bool value);

    Text text;

    [[nodiscard]] Size terminal_size() const;

private:
    bool graphical_mode_ = false;
};

#endif //SCENE_HH_
