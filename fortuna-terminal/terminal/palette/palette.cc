#include "palette.hh"

void palette_init(Palette& palette)
{
    palette[COLOR_BLACK] =      (Color) { 0x1a, 0x1c, 0x2c };
    palette[COLOR_PURPLE] =     (Color) { 0x5d, 0x27, 0x5d };
    palette[COLOR_RED] =        (Color) { 0xc1, 0x4e, 0x63 };
    palette[COLOR_ORANGE] =     (Color) { 0xef, 0x7d, 0x57 };
    palette[COLOR_YELLOW] =     (Color) { 0xff, 0xcd, 0x75 };
    palette[COLOR_LIME] =       (Color) { 0xa7, 0xf0, 0x70 };
    palette[COLOR_GREEN] =      (Color) { 0x38, 0xb7, 0x64 };
    palette[COLOR_TURQUOISE] =  (Color) { 0x25, 0x71, 0x79 };
    palette[COLOR_DARK_BLUE] =  (Color) { 0x29, 0x36, 0x6f };
    palette[COLOR_BLUE] =       (Color) { 0x3b, 0x5d, 0xc9 };
    palette[COLOR_LIGHT_BLUE] = (Color) { 0x41, 0xa6, 0xf6 };
    palette[COLOR_CYAN] =       (Color) { 0x73, 0xef, 0xf7 };
    palette[COLOR_WHITE] =      (Color) { 0xf4, 0xf4, 0xf4 };
    palette[COLOR_LIGHT_GRAY] = (Color) { 0x94, 0xb0, 0xc2 };
    palette[COLOR_GRAY] =       (Color) { 0x56, 0x6c, 0x86 };
    palette[COLOR_DARK_GRAY] =  (Color) { 0x33, 0x3c, 0x57 };
}
