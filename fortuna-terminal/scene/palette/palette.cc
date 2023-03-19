#include "palette.hh"

void palette_init(Palette& palette)
{
    palette[COLOR_DARK_BLACK] =  (Color) { 0x00, 0x00, 0x00 };
    palette[COLOR_BLACK] =       (Color) { 0x0b, 0x0a, 0x0d };
    palette[COLOR_LIGHT_BLACK] = (Color) { 0x16, 0x15, 0x24 };
    palette[COLOR_DARK_SLATE] =  (Color) { 0x22, 0x26, 0x40 };
    palette[COLOR_SLATE] =       (Color) { 0x2b, 0x40, 0x57 };
    palette[COLOR_DARK_CYAN] =   (Color) { 0x30, 0x65, 0x66 };
    palette[COLOR_DARK_GREEN] =  (Color) { 0x34, 0xa8, 0x70 };
    palette[COLOR_GREEN] =       (Color) { 0x49, 0xf2, 0x5a };
    palette[COLOR_LIGHT_GREEN] = (Color) { 0xa4, 0xff, 0x63 };
    palette[COLOR_YELLOW] =      (Color) { 0xff, 0xf2, 0x40 };
    palette[COLOR_ORANGE] =      (Color) { 0xf2, 0xa5, 0x3f };
    palette[COLOR_BROWN] =       (Color) { 0xcc, 0x7a, 0x47 };
    palette[COLOR_RED] =         (Color) { 0xf5, 0x40, 0x25 };
    palette[COLOR_DARK_RED] =    (Color) { 0xa6, 0x3a, 0x3a };
    palette[COLOR_DARK_BROWN] =  (Color) { 0x99, 0x53, 0x48 };
    palette[COLOR_DARK_PINK] =   (Color) { 0x73, 0x37, 0x58 };
    palette[COLOR_DARK_PURPLE] = (Color) { 0x4d, 0x2a, 0x49 };
    palette[COLOR_INDIGO] =      (Color) { 0x46, 0x34, 0x6a };
    palette[COLOR_PURPLE] =      (Color) { 0x8c, 0x2e, 0xb8 };
    palette[COLOR_PINK] =        (Color) { 0xf2, 0x61, 0xda };
    palette[COLOR_LIGHT_PINK] =  (Color) { 0xff, 0xa8, 0xd4 };
    palette[COLOR_LIGHT_CYAN] =  (Color) { 0xb3, 0xdf, 0xff };
    palette[COLOR_CYAN] =        (Color) { 0x70, 0xa5, 0xfa };
    palette[COLOR_LIGHT_BLUE] =  (Color) { 0x40, 0x7c, 0xff };
    palette[COLOR_BLUE] =        (Color) { 0x1f, 0x50, 0xcc };
    palette[COLOR_DARK_BLUE] =   (Color) { 0x21, 0x3e, 0xa6 };
    palette[COLOR_NAVY_BLUE] =   (Color) { 0x27, 0x2f, 0x66 };
    palette[COLOR_DARK_GRAY] =   (Color) { 0x41, 0x45, 0x58 };
    palette[COLOR_GRAY] =        (Color) { 0x6d, 0x70, 0x78 };
    palette[COLOR_LIGHT_GRAY] =  (Color) { 0x89, 0x8b, 0x8c };
    palette[COLOR_DARK_WHITE] =  (Color) { 0xbb, 0xbd, 0xbf };
    palette[COLOR_WHITE] =       (Color) { 0xff, 0xff, 0xff };
}