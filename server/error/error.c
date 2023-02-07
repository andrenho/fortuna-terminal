#include "error.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <SDL2/SDL.h>

void error_check(int f)
{
    switch (f) {
        case OK:
            break;
        case ERR_SDL:
            fprintf(stderr, "SDL error: %s\n", SDL_GetError());
            break;
        case ERR_FAIL:
            break;
        case ERR_NOT_IMPLEMENTED:
            fprintf(stderr, "Function not implemented.\n");
            break;
    }

    if (f > 0)
        fprintf(stderr, "Error reported by the glibc: %s", strerror(errno));

    if (f != OK)
        exit(EXIT_FAILURE);
}