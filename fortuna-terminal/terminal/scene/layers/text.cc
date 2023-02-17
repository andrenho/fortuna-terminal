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
