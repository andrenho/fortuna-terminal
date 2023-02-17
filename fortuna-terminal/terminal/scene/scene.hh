#ifndef SCENE_HH_
#define SCENE_HH_

#include "layers/text.hh"
#include "../../common/noncopyable.hh"
#include "common/geometry.hh"

enum class SceneMode {
    Text80Columns, TextAndGraphics,
};

struct Scene : NonCopyable {
    Text text;

    SceneMode mode = SceneMode::Text80Columns;

    Size terminal_size() const;
};

#endif //SCENE_HH_
