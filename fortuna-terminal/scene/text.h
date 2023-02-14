#ifndef TEXT_H_
#define TEXT_H_

#include <fortuna-protocol.h>

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
    CharAttrib a;
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

    CharAttrib   attrib;

    Palette      palette;
} Text;

int  text_init(Text* text);
Char text_get(Text* text, unsigned int y, unsigned int x);
void text_update_blink(Text* text);

// void text_add_char(Text* text, uint8_t c, uint8_t color);
void text_set_char(Text* text, unsigned int y, unsigned int x, uint8_t c, CharAttrib a);

void text_move_cursor_to(Text* text, unsigned int y, unsigned int x);
void text_move_cursor_down_scroll(Text* text);

void text_set_color(Text* text, uint8_t color);
void text_print(Text* text, const char* str);

void text_attrib_reset(Text* text);

#endif //TEXT_H_
