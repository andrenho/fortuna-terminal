#include "error.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>

#include <SDL2/SDL.h>

static char current_error_[2048];
static bool error_ui_requested_ = false;

void error_set(const char* fmt, ...)
{
    if (fmt[0] == '\0')
        return;

    va_list args;

    va_start(args, fmt);
    vsnprintf(current_error_, sizeof(current_error_), fmt, args);
    va_end(args);
}

static void error_to_str(FT_Result result, char* str, size_t sz)
{
    switch (result) {
        case FT_OK:
            abort();
        case FT_ERR_APP:
            strncpy(str, current_error_, sz);
            break;
        case FT_ERR_LIBC:
            snprintf(str, sz, "%s: %s", current_error_, strerror(errno));
            break;
        case FT_ERR_SDL:
            snprintf(str, sz, "%s: %s", current_error_, SDL_GetError());
            break;
    }
}

void error_print(FT_Result result)
{
    char buf[sizeof current_error_];
    error_to_str(result, buf, sizeof buf);
    fprintf(stderr, "\n\e[1;91m%s\e[0m\n", buf);
}

void error_ui(FT_Result result)
{
    char buf[sizeof current_error_];
    error_to_str(result, buf, sizeof buf);
    error_ui_requested_ = true;
}

bool error_ui_requested(char* str, size_t sz)
{
    if (error_ui_requested_)
        strncpy(str, current_error_, sz);
    return error_ui_requested_;
}

void error_clear()
{
    error_ui_requested_ = false;
    current_error_[0] = '\0';
}