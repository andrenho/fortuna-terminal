#include "error.h"

#include <stdio.h>
#include <stdlib.h>

#include <SDL2/SDL.h>

void error_check(int f)
{
    switch (f) {
        case OK:
            break;
        case ERR_SDL:
            fprintf(stderr, "SDL error: %s\n", SDL_GetError());
    }

    if (f != OK)
        exit(EXIT_FAILURE);
}