#ifndef SCENE_H_
#define SCENE_H_

#include "text.h"

typedef struct {
    Text text;
} Scene;

int scene_init(Scene* scene);

#endif //SCENE_H_
