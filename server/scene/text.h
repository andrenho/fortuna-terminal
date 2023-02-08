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

void text_add_char(Text* text, uint8_t c);

void text_move_cursor_relative(Text* text, int y, int x);
void text_move_cursor_to(Text* text, unsigned int y, unsigned int x);
void text_set_color(Text* text, uint8_t color);
void text_clear_screen(Text* text);
void text_reset_formatting(Text* text);
void text_clear_to_beginning_of_line(Text* text);
void text_clear_to_end_of_line(Text* text);
void text_clear_line(Text* text);
void text_set_insertion_mode(Text* text, bool active);
void text_set_scroll_region(Text* text, uint8_t start, uint8_t end);
void text_delete_char_under_cursor(Text* text);

#endif //TEXT_H_
