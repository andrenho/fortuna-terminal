#ifndef TEXT_H_
#define TEXT_H_

#include <stdbool.h>
#include <stdint.h>
#include "palette.h"

#define MAX_COLUMNS 80
#define MAX_LINES   25

typedef struct {
    uint8_t  x;
    uint8_t  y;
    uint8_t  color;
    bool     visible;
    bool     blink_state;
} Cursor;

typedef struct {
    uint8_t c;
    uint8_t color;
} Char;

typedef struct {
    Char         matrix[MAX_COLUMNS * MAX_LINES];
    Cursor       cursor;
    uint64_t     cursor_last_blink;

    unsigned int columns;
    unsigned int lines;
    unsigned int scroll_start;
    unsigned int scroll_end;
    bool         insertion_mode;

    uint8_t      fg_color;

    Palette      palette;
} Text;

int  text_init(Text* text);
Char text_get(Text* text, unsigned int y, unsigned int x);
void text_update_blink(Text* text);

#endif //TEXT_H_
