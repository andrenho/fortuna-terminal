#include "text.h"

#include <SDL2/SDL.h>

#include "palette.h"

#define max(a,b) ({ __typeof__ (a) _a = (a); __typeof__ (b) _b = (b); _a > _b ? _a : _b; })
#define min(a,b) ({ __typeof__ (a) _a = (a); __typeof__ (b) _b = (b); _a < _b ? _a : _b; })

static CharAttrib default_attr = { .color = COLOR_WHITE, .reverse = false };

static void text_reset_blink(Text* text)
{
    text->cursor.blink_state = true;
    text->cursor_last_blink = SDL_GetTicks64();
}

void text_set_char(Text* text, unsigned int y, unsigned int x, uint8_t c, CharAttrib a)
{
    text->matrix[y * text->columns + x] = (Char) { c, a };
    text_reset_blink(text);
}

Char text_get(Text* text, unsigned int y, unsigned int x)
{
    return text->matrix[y * text->columns + x];
}

void text_copy(Text* text, unsigned int dest_y, unsigned int dest_x, unsigned int src_y, unsigned int src_x)
{
    text->matrix[dest_y * text->columns + dest_x] = text->matrix[src_y * text->columns + src_x];
}

int text_init(Text* text)
{
    text->columns = 40;
    text->lines = 25;
    text->scroll_start = 1;
    text->scroll_end = text->lines;
    text->insertion_mode = false;

    text->cursor = (Cursor) {
        .x = 0,
        .y = 0,
        .color = COLOR_ORANGE,
        .visible = true,
        .blink_state = true,
    };
    text->cursor_last_blink = SDL_GetTicks();

    text->attrib = default_attr;

    for (size_t y = 0; y < text->lines; ++y)
        for (size_t x = 0; x < text->columns; ++x)
            text_set_char(text, y, x, ' ', text->attrib);
            // text_set(text, y, x, y * text->columns + x);

    palette_init(&text->palette);

    return 0;
}

void text_attrib_reset(Text* text)
{
    text->attrib = default_attr;
}

static void text_advance_line(Text* text)
{
    text->cursor.x = 0;
    text_move_cursor_down_scroll(text);
}

static void text_advance_cursor(Text* text)
{
    ++text->cursor.x;
    if (text->cursor.x >= text->columns)
        text_advance_line(text);
    text_reset_blink(text);
}

void text_add_char(Text* text, uint8_t c)
{
    text_set_char(text, text->cursor.y, text->cursor.x, c, text->attrib);
    text_advance_cursor(text);
    text_reset_blink(text);
}

void text_set_color(Text* text, uint8_t color)
{
    text->attrib.color = color;
}

void text_print(Text* text, const char* str)
{
    for (const char* s = str; *s; ++s)
        text_add_char(text, *s);
}

void text_update_blink(Text* text)
{
    if (SDL_GetTicks64() > text->cursor_last_blink + 600) {
        text->cursor.blink_state = !text->cursor.blink_state;
        text->cursor_last_blink = SDL_GetTicks64();
    }
}

void text_move_cursor_to(Text* text, unsigned int y, unsigned int x)
{
    text->cursor.x = max(0U, min(text->columns, x));
    text->cursor.y = max(0U, min(text->lines, y));
    text_reset_blink(text);
}

void text_move_cursor_down_scroll(Text* text)
{
    ++text->cursor.y;
    if (text->cursor.y >= text->lines) {
        for (size_t y = (text->scroll_start - 1); y < (text->scroll_end - 1); ++y) {
            for (size_t x = 0; x < text->columns; ++x)
                text_copy(text, y, x, y + 1, x);
        }
        --text->cursor.y;
        for (size_t x = 0; x < text->columns; ++x)
            text_set_char(text, text->cursor.y, x, ' ', default_attr);
    }
    text_reset_blink(text);
}

