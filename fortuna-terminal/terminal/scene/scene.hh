#ifndef SCENE_HH_
#define SCENE_HH_

#include "layers/text.hh"
#include "../../common/noncopyable.hh"

struct Scene : NonCopyable {
    Text text;
};

#endif //SCENE_HH_
