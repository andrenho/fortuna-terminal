#ifndef COLOR_HH_
#define COLOR_HH_

#include <cstdint>

struct Color {
    uint8_t r, g, b;
};

// palette: https://lospec.com/palette-list/koni32

enum : uint8_t {
    COLOR_DARK_BLACK,
    COLOR_BLACK,
    COLOR_LIGHT_BLACK,
    COLOR_DARK_SLATE,
    COLOR_SLATE,

    COLOR_DARK_CYAN,
    COLOR_DARK_GREEN,
    COLOR_GREEN,
    COLOR_LIGHT_GREEN,
    COLOR_YELLOW,

    COLOR_ORANGE,
    COLOR_BROWN,
    COLOR_RED,
    COLOR_DARK_RED,
    COLOR_DARK_BROWN,

    COLOR_DARK_PINK,
    COLOR_DARK_PURPLE,
    COLOR_INDIGO,
    COLOR_PURPLE,
    COLOR_PINK,

    COLOR_LIGHT_PINK,
    COLOR_LIGHT_CYAN,
    COLOR_CYAN,
    COLOR_LIGHT_BLUE,
    COLOR_BLUE,

    COLOR_DARK_BLUE,
    COLOR_NAVY_BLUE,
    COLOR_DARK_GRAY,
    COLOR_GRAY,
    COLOR_LIGHT_GRAY,

    COLOR_DARK_WHITE,
    COLOR_WHITE,

    PALETTE_SZ  // not a color
};

#endif //COLOR_HH_
