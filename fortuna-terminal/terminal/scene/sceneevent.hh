#ifndef SCENEEVENT_HH_
#define SCENEEVENT_HH_

#include "../../../fortuna-protocol/fortuna-protocol.h"
#include "../../common/noncopyable.hh"

struct SceneEvent : NonCopyable {
    SceneEvent(unsigned int _scene, FP_Message _message) : scene(_scene), message(_message) {}

    unsigned int scene;
    FP_Message   message;
};

#endif //SCENEEVENT_HH_
