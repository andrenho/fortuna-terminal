#ifndef PAINTER_H_
#define PAINTER_H_

#include <SDL2/SDL.h>

#include "scene/scene.h"
#include "options/options.h"

int painter_init(SDL_Window *window);
int painter_draw(Scene* scene);
int painter_destroy();

#endif //PAINTER_H_
