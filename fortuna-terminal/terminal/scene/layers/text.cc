#include "text.hh"
#include "terminal/scene/scene.hh"

#include <SDL2/SDL.h>

using namespace std::chrono_literals;

void Text::set_scene_mode(SceneMode scene_mode)
{
    switch (scene_mode) {
        case SceneMode::Text80Columns:
            columns_ = Columns_80Columns;
            lines_ = Lines_80Columns;
            break;
        case SceneMode::TextAndGraphics:
            columns_ = Columns_40Columns;
            lines_ = Lines_40Columns;
            break;
    }

    matrix_ = std::make_unique<Char[]>(columns_ * lines_);

    reset_attributes();

    for (size_t i = 0; i < (columns_ * lines_); ++i)
        matrix_[i] = { ' ', current_attrib_ };
        // matrix_[i] = { (uint8_t) i, { (uint8_t) (i % 15),  i % 20 == 0 } };
}

Char const &Text::get(size_t line, size_t column) const
{
    return matrix_[line * columns_ + column];
}

Char &Text::get(size_t line, size_t column)
{
    return matrix_[line * columns_ + column];
}

void Text::set(size_t line, size_t column, uint8_t c)
{
    get(line, column) = { c, current_attrib_ };
}

void Text::set(size_t line, size_t column, Char c)
{
    get(line, column) = c;
}

void Text::reset_attributes()
{
    current_attrib_ = { COLOR_WHITE, false };
}

void Text::reset_blink()
{
    cursor_.blink_state = true;
    cursor_.last_blink = Time::now();
}

void Text::update_blink()
{
    if (Time::now() > cursor_.last_blink + 600ms) {
        cursor_.blink_state = !cursor_.blink_state;
        cursor_.last_blink = Time::now();
    }
}

void Text::write(uint8_t c)
{
    if (c == '\n') {
        advance_line();
    } else {
        set(cursor_.y, cursor_.x, c);
        advance_cursor();
    }
    reset_blink();
}

void Text::advance_cursor()
{
    ++cursor_.x;
    if (cursor_.x > columns_)
        advance_line();
    reset_blink();
}

void Text::advance_line()
{
    cursor_.x = 0;
    move_cursor_one_line_down();
}

void Text::move_cursor_one_line_down()
{
    ++cursor_.y;
    if (cursor_.y > lines_) {
        for (size_t y = 0; y < (lines_ - 1); ++y)
            std::copy(&get(y + 1, 0), &get(y + 1, columns_ - 1), &get(y, 0));
        --cursor_.y;
        std::fill(&get(cursor_.y, 0), &get(cursor_.y, columns_ - 1), Char { ' ', current_attrib_ });
    }
    reset_blink();
}

