#include "text.h"

#include <SDL2/SDL.h>

#include "palette.h"

static void text_set(Text* text, unsigned int y, unsigned int x, uint8_t c)
{
    text->matrix[y * text->columns + x] = (Char) { c, text->fg_color };
}

Char text_get(Text* text, unsigned int y, unsigned int x)
{
    return text->matrix[y * text->columns + x];
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

    uint8_t i = 0;
    for (size_t y = 0; y < text->lines; ++y)
        for (size_t x = 0; x < text->columns; ++x)
            // text_set(text, y, x, ' ');
            text_set(text, y, x, i++);

    palette_init(&text->palette);

    return 0;
}

void text_update_blink(Text* text)
{
    if (SDL_GetTicks64() > text->cursor_last_blink + 600) {
        text->cursor.blink_state = !text->cursor.blink_state;
        text->cursor_last_blink = SDL_GetTicks64();
    }
}
