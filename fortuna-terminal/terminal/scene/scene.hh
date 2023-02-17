#ifndef SCENE_HH_
#define SCENE_HH_

#include "layers/text.hh"
#include "../../common/noncopyable.hh"
#include "common/geometry.hh"
#include "scene_mode.hh"

struct Scene : NonCopyable {
    Scene();

    Text text;

    [[nodiscard]] Size terminal_size() const;

private:
    SceneMode mode = SceneMode::Text80Columns;
};

#endif //SCENE_HH_
