#include "palette.h"

int palette_init(Palette* palette)
{
    palette->color[COLOR_BLACK] =      (Color) { 0x1a, 0x1c, 0x2c };
    palette->color[COLOR_PURPLE] =     (Color) { 0x5d, 0x27, 0x5d };
    palette->color[COLOR_RED] =        (Color) { 0xb1, 0x3e, 0x53 };
    palette->color[COLOR_ORANGE] =     (Color) { 0xef, 0x7d, 0x57 };
    palette->color[COLOR_YELLOW] =     (Color) { 0xff, 0xcd, 0x75 };
    palette->color[COLOR_LIME] =       (Color) { 0xa7, 0xf0, 0x70 };
    palette->color[COLOR_GREEN] =      (Color) { 0x38, 0xb7, 0x64 };
    palette->color[COLOR_TURQUOISE] =  (Color) { 0x25, 0x71, 0x79 };
    palette->color[COLOR_DARK_BLUE] =  (Color) { 0x29, 0x36, 0x6f };
    palette->color[COLOR_BLUE] =       (Color) { 0x3b, 0x5d, 0xc9 };
    palette->color[COLOR_LIGHT_BLUE] = (Color) { 0x41, 0xa6, 0xf6 };
    palette->color[COLOR_CYAN] =       (Color) { 0x73, 0xef, 0xf7 };
    palette->color[COLOR_WHITE] =      (Color) { 0xf4, 0xf4, 0xf4 };
    palette->color[COLOR_LIGHT_GRAY] = (Color) { 0x94, 0xb0, 0xc2 };
    palette->color[COLOR_GRAY] =       (Color) { 0x56, 0x6c, 0x86 };
    palette->color[COLOR_DARK_GRAY] =  (Color) { 0x33, 0x3c, 0x57 };
    return 0;
}