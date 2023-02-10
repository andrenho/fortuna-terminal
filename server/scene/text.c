#include "text.h"

#include <SDL2/SDL.h>

#include "palette.h"

#define max(a,b) ({ __typeof__ (a) _a = (a); __typeof__ (b) _b = (b); _a > _b ? _a : _b; })
#define min(a,b) ({ __typeof__ (a) _a = (a); __typeof__ (b) _b = (b); _a < _b ? _a : _b; })

static void text_set(Text* text, unsigned int y, unsigned int x, uint8_t c)
{
    text->matrix[y * text->columns + x] = (Char) { c, text->fg_color };
}

Char text_get(Text* text, unsigned int y, unsigned int x)
{
    return text->matrix[y * text->columns + x];
}

void text_copy(Text* text, unsigned int dest_y, unsigned int dest_x, unsigned int src_y, unsigned int src_x)
{
    text->matrix[dest_y * text->columns + dest_x] = text->matrix[src_y * text->columns + src_x];
}

static void text_reset_blink(Text* text)
{
    text->cursor.blink_state = true;
    text->cursor_last_blink = SDL_GetTicks64();
}

int text_init(Text* text)
{
    text->columns = 40;
    text->lines = 25;
    text->scroll_start = 1;
    text->scroll_end = text->lines;
    text->insertion_mode = false;

    text->fg_color = COLOR_WHITE;

    text->cursor = (Cursor) {
        .x = 0,
        .y = 0,
        .color = COLOR_ORANGE,
        .visible = true,
        .blink_state = true,
    };
    text->cursor_last_blink = SDL_GetTicks();

    for (size_t y = 0; y < text->lines; ++y)
        for (size_t x = 0; x < text->columns; ++x)
            text_set(text, y, x, ' ');
            // text_set(text, y, x, y * text->columns + x);

    palette_init(&text->palette);

    return 0;
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

static void text_push_line_forward(Text* text)
{
    for (int x = (int) text->columns - 2; x >= text->cursor.x; --x)
        text_copy(text, text->cursor.y, x + 1, text->cursor.y, x);
    text_reset_blink(text);
}

static void text_tab(Text* text)
{
    size_t next_tab = ((text->cursor.x / 8) * 8) + 8;
    if (next_tab >= text->columns) {
        next_tab = 0;
        text_advance_line(text);
    }
    text->cursor.x = next_tab;
    text_reset_blink(text);
}

static void text_backspace(Text* text)
{
    if (text->cursor.x > 0) {
        --text->cursor.x;
        text_add_char(text, ' ');  // TODO - what about insertion mode?
        --text->cursor.x;
    }
    text_reset_blink(text);
}

void text_add_char(Text* text, uint8_t c)
{
    switch (c) {
        /*
        case '\n':
            text_advance_line(text);
            break;
        */
        case '\t':
            text_tab(text);
            break;
        /*
        case '\b':
            text_backspace(text);
            break;
        */
        default:
            if (text->insertion_mode)
                text_push_line_forward(text);
            text_set(text, text->cursor.y, text->cursor.x, c);
            text_advance_cursor(text);
    }
    text_reset_blink(text);
}

void text_update_blink(Text* text)
{
    if (SDL_GetTicks64() > text->cursor_last_blink + 600) {
        text->cursor.blink_state = !text->cursor.blink_state;
        text->cursor_last_blink = SDL_GetTicks64();
    }
}

void text_move_cursor_relative(Text* text, int y, int x)
{
    text->cursor.x = max(0, min((int) text->columns, text->cursor.x + x));
    text->cursor.y = max(0, min((int) text->lines, text->cursor.y + y));
    text_reset_blink(text);
}

void text_move_cursor_to(Text* text, unsigned int y, unsigned int x)
{
    text->cursor.x = max(0U, min(text->columns, x - 1));
    text->cursor.y = max(0U, min(text->lines, y - 1));
    text_reset_blink(text);
}

void text_set_color(Text* text, uint8_t color)
{
    text->fg_color = (color % PALETTE_SZ);
}

void text_clear_screen(Text* text)
{
    for (size_t i = 0; i < (text->columns * text->lines); ++i)
        text->matrix[i] = (Char) { ' ', text->fg_color };
    text_reset_blink(text);
}

void text_reset_formatting(Text* text)
{
    text->fg_color = COLOR_WHITE;
}

void text_clear_to_beginning_of_line(Text* text)
{
    for (size_t x = 0; x <= text->cursor.x; ++x)
        text_set(text, text->cursor.y, x, ' ');
    text_reset_blink(text);
}

void text_clear_to_end_of_line(Text* text)
{
    for (size_t x = text->cursor.x; x < text->columns; ++x)
        text_set(text, text->cursor.y, x, ' ');
    text_reset_blink(text);
}

void text_clear_to_end_of_screen(Text* text)
{
    for (size_t y = text->cursor.y; y < text->lines; ++y)
        for (size_t x = 0; x <= text->columns; ++x)
            text_set(text, y, x, ' ');
}

void text_clear_line(Text* text)
{
    for (size_t x = 0; x < text->columns; ++x)
        text_set(text, text->cursor.y, x, ' ');
    text_reset_blink(text);
}

void text_set_insertion_mode(Text* text, bool active)
{
    text->insertion_mode = active;
}

void text_set_scroll_region(Text* text, uint8_t start, uint8_t end)
{
    text->scroll_start = start;
    text->scroll_end = end;
}

void text_delete_char_under_cursor(Text* text)
{
    text_set(text, text->cursor.y, text->cursor.x, ' ');
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
            text_set(text, text->cursor.y, x, ' ');
    }
    text_reset_blink(text);
}

void text_move_cursor_bol(Text* text)
{
    text->cursor.x = 0;
}
