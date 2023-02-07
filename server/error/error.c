#include "error.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <SDL2/SDL.h>

void error_check(int f)
{
    switch (f) {
        case OK:
        case ERR_NO_DATA:
            break;
        case ERR_SDL:
            fprintf(stderr, "SDL error: %s\n", SDL_GetError());
            break;
        case ERR_FAIL:
            break;
        case ERR_NOT_IMPLEMENTED:
            fprintf(stderr, "Function not implemented.\n");
            break;
        case ERR_LIBC:
            fprintf(stderr, "Error reported by the glibc: %s", strerror(errno));
            break;
        case ERR_BUF_OVERFLOW:
            fprintf(stderr, "Buffer overflow.\n");
            break;
        case ERR_COMMUNICATION_CLOSED:
            fprintf(stderr, "Communication channel closed.\n");
            break;
        default:
            fprintf(stderr, "An undefined error happenend.\n");
    }
}