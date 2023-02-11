#ifndef UI_H_
#define UI_H_

#include <stdbool.h>
#include "../options/options.h"
#include "scene/scene.h"

#define GRAPHICS_W 256
#define GRAPHICS_H 256

int  ui_init();
bool ui_running();
int  ui_do_events();
int  ui_draw(Scene* scene);
int  ui_destroy();

#endif //UI_H_