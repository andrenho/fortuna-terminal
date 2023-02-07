#include "scene.h"

int scene_init(Scene* scene)
{
    text_init(&scene->text);
    return 0;
}