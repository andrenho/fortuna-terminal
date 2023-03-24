#include "textlayer.hh"

using namespace std::chrono_literals;

TextLayer::TextLayer(Mode mode)
    : mode_(mode)
{
    set_mode(mode);
}

Char const &TextLayer::get_char(size_t row, size_t column) const
{
    return matrix_[row * columns_ + column];
}

void TextLayer::update_char(size_t row, size_t column, Char c)
{
    matrix_[row * columns_ + column] = c;
    reset_blink();
}

void TextLayer::update_cell(std::vector<Cell> const &cells)
{
    for (Cell const& cell: cells)
        matrix_[cell.line * columns_ + cell.column] = cell.chr;
    reset_blink();
}

void TextLayer::move_cursor_to(size_t line, size_t column)
{
    cursor_.y = line;
    cursor_.x = column;
    reset_blink();
}

void TextLayer::reset_blink()
{
    cursor_.blink_state = true;
    cursor_.last_blink = Time::now();
}

void TextLayer::update_blink()
{
    if (Time::now() > cursor_.last_blink + 600ms) {
        cursor_.blink_state = !cursor_.blink_state;
        cursor_.last_blink = Time::now();
    }
}

void TextLayer::reset()
{
    for (size_t i = 0; i < (columns_ * lines_); ++i)
        matrix_[i] = { ' ', { COLOR_WHITE, false, true, } };
}

void TextLayer::set_mode(Mode mode)
{
    mode_ = mode;

    if (mode == Mode::Text) {
        columns_ = Columns_80Columns;
        lines_ = Lines_80Columns;
    } else {
        columns_ = Columns_40Columns;
        lines_ = Lines_40Columns;
    }

    matrix_ = std::make_unique<Char[]>(columns_ * lines_);
}

void TextLayer::write_text(size_t row, size_t column, std::string const &text, Char::Attrib const& attrib)
{
    if (row >= lines_)
        return;

    for (uint8_t c: text) {
        if (column > columns_)
            return;
        matrix_[(row * columns_) + (column++)] = { c, attrib };
    }
}
