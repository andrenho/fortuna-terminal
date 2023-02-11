#ifndef PALETTE_H_
#define PALETTE_H_

#include <stdint.h>

#define PALETTE_SZ 16

typedef enum {
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
    COLOR_DARK_GRAY
} ColorEnum;

typedef struct __attribute__((packed)) {
    uint8_t r, g, b;
} Color;

typedef struct {
    Color color[PALETTE_SZ];
} Palette;

int palette_init(Palette* palette);

#endif //PALETTE_H_
