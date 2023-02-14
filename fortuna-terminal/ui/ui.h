#ifndef UI_H_
#define UI_H_

#include <stdbool.h>
#include "../options/options.h"
#include "scene/scene.h"

#define GRAPHICS_W 256
#define GRAPHICS_H 256

FT_Result ui_init();
bool      ui_running();
void      ui_do_events();
void      ui_wait_for_keypress();
void      ui_draw(Scene* scene);
void      ui_destroy();

#endif //UI_H_
