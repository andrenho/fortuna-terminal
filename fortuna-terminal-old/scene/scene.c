#include "scene.h"

int scene_init(Scene* scene)
{
    scene->bg_color = COLOR_BLACK;
    palette_init(&scene->palette);

    text_init(&scene->text);
    return 0;
}