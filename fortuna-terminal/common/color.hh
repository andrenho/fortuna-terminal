#ifndef COLOR_HH_
#define COLOR_HH_

#include <cstdint>

struct Color {
    uint8_t r, g, b;
};

enum : uint8_t {
    COLOR_BLACK,
    COLOR_PURPLE,
    COLOR_RED,
    COLOR_ORANGE,
    COLOR_YELLOW,
    COLOR_LIME,
    COLOR_GREEN,
    COLOR_TURQUOISE,
    COLOR_DARK_BLUE,
    COLOR_BLUE,
    COLOR_LIGHT_BLUE,
    COLOR_CYAN,
    COLOR_WHITE,
    COLOR_LIGHT_GRAY,
    COLOR_GRAY,
    COLOR_DARK_GRAY,
    PALETTE_SZ  // not a color
};

#endif //COLOR_HH_
