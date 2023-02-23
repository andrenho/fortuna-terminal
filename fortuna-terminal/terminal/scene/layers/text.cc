#include "text.hh"
#include "terminal/scene/scene.hh"

#include <SDL2/SDL.h>

using namespace std::chrono_literals;

Text::Text(Mode mode)
{
    set_mode(mode);
}

Char const &Text::get(size_t line, size_t column) const
{
    return matrix_[line * columns_ + column];
}

void Text::set(size_t line, size_t column, Char c)
{
    matrix_[line * columns_ + column] = c;
    reset_blink();
}

void Text::set(std::vector<Cell> const &cells)
{
    for (Cell const& cell: cells)
        matrix_[cell.line * columns_ + cell.column] = cell.chr;
    reset_blink();
}

void Text::move_cursor_to(size_t line, size_t column)
{
    cursor_.y = line;
    cursor_.x = column;
    reset_blink();
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

void Text::reset()
{
    for (size_t i = 0; i < (columns_ * lines_); ++i)
        matrix_[i] = { ' ', { COLOR_WHITE, false, true, } };
}

void Text::set_mode(Mode mode)
{
    if (mode == Mode::Text) {
        columns_ = Columns_80Columns;
        lines_ = Lines_80Columns;
    } else {
        columns_ = Columns_40Columns;
        lines_ = Lines_40Columns;
    }

    matrix_ = std::make_unique<Char[]>(columns_ * lines_);

    for (size_t i = 0; i < (columns_ * lines_); ++i)
        matrix_[i] = { ' ', { COLOR_WHITE, false, true, } };
    // matrix_[i] = { (uint8_t) i, { (uint8_t) (i % 15),  i % 20 == 0 } };
}
