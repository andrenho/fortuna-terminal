#ifndef PALETTE_HH_
#define PALETTE_HH_

#include <cstdint>

struct Color {
    uint8_t r, g, b;

    static constexpr uint8_t BLACK      = 0X0;
    static constexpr uint8_t PURPLE     = 0X1;
    static constexpr uint8_t RED        = 0X2;
    static constexpr uint8_t ORANGE     = 0X3;
    static constexpr uint8_t YELLOW     = 0X4;
    static constexpr uint8_t LIME       = 0X5;
    static constexpr uint8_t GREEN      = 0X6;
    static constexpr uint8_t TURQUOISE  = 0X7;
    static constexpr uint8_t DARK_BLUE  = 0X8;
    static constexpr uint8_t BLUE       = 0X9;
    static constexpr uint8_t LIGHT_BLUE = 0XA;
    static constexpr uint8_t CYAN       = 0XB;
    static constexpr uint8_t WHITE      = 0XC;
    static constexpr uint8_t LIGHT_GRAY = 0XD;
    static constexpr uint8_t GRAY       = 0XE;
    static constexpr uint8_t DARK_GRAY  = 0XF;
};

struct Palette {
    static constexpr int SIZE = 16;

    Color color[SIZE] = {
        { 0x1a, 0x1c, 0x2c },   // BLACK
        { 0x5d, 0x27, 0x5d },   // PURPLE
        { 0xb1, 0x3e, 0x53 },   // RED
        { 0xef, 0x7d, 0x57 },   // ORANGE
        { 0xff, 0xcd, 0x75 },   // YELLOW
        { 0xa7, 0xf0, 0x70 },   // LIME
        { 0x38, 0xb7, 0x64 },   // GREEN
        { 0x25, 0x71, 0x79 },   // TURQUOISE
        { 0x29, 0x36, 0x6f },   // DARK_BLUE
        { 0x3b, 0x5d, 0xc9 },   // BLUE
        { 0x41, 0xa6, 0xf6 },   // LIGHT_BLUE
        { 0x73, 0xef, 0xf7 },   // CYAN
        { 0xf4, 0xf4, 0xf4 },   // WHITE
        { 0x94, 0xb0, 0xc2 },   // LIGHT_GRAY
        { 0x56, 0x6c, 0x86 },   // GRAY
        { 0x33, 0x3c, 0x57 },   // DARK_GRAY
    };
};

#endif //PALETTE_HH_
