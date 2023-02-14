#ifndef PAINTER_H_
#define PAINTER_H_

#include <SDL2/SDL.h>

#include "scene/scene.h"
#include "options/options.h"

FT_Result painter_init(SDL_Window *window);
void      painter_draw(Scene* scene);
void      painter_destroy();

#endif //PAINTER_H_
