#include "protocol_debug.h"
#include "../options/options.h"

#include <stdint.h>
#include <stdio.h>

void debug_char(uint8_t c)
{
    if (options.debug_mode) {
        if (c < 32 || c >= 128)
            printf("[%02X]", c);
        else
            printf("%c", c);
        if (c == '\n')
            printf("\n");
        fflush(stdout);
    }
}

void debug_special_0(const char* str)
{
    if (options.debug_mode) {
#if _WIN32
        printf(" (%s)\n", str);
#else
        printf("\e[1;35m (%s)\e[0m\n", str);
#endif
        fflush(stdout);
    }
}

void debug_special_1(const char* str, int p1)
{
    if (options.debug_mode) {
#if _WIN32
        printf(" (%s %d)\n", str, p1);
#else
        printf("\e[1;35m (%s %d)\e[0m\n", str, p1);
#endif
        fflush(stdout);
    }
}

void debug_special_2(const char* str, int p1, int p2)
{
    if (options.debug_mode) {
#if _WIN32
        printf(" (%s %d %d)\n", str, p1, p2);
#else
        printf("\e[1;35m (%s %d %d)\e[0m\n", str, p1, p2);
#endif
        fflush(stdout);
    }
}

