#include "textlayer.hh"

#include <memory>

using namespace std::chrono_literals;

TextLayer::TextLayer()
{
    cursor_last_blink_ = std::chrono::high_resolution_clock::now();
    matrix_ = std::make_unique<Char[]>(columns_ * lines_);
    for (size_t i = 0; i < (columns_ * lines_); ++i)
        matrix_[i] = { ' ' /* (uint8_t) (i % 256) */, Color::WHITE };
}

void TextLayer::update_blink()
{
    if (std::chrono::high_resolution_clock::now() > cursor_last_blink_ + 600ms) {
        cursor_.blink_state = !cursor_.blink_state;
        cursor_last_blink_ = std::chrono::high_resolution_clock::now();
    }
}

void TextLayer::add_char(uint8_t c)
{
    Char& ch = chr(cursor_.y, cursor_.x);
    ch.chr = c;
    ch.color = foreground_color_;

    text_advance_cursor();
    cursor_.blink_state = true;
    cursor_last_blink_ = std::chrono::high_resolution_clock::now();
}

void TextLayer::text_advance_line()
{
    /*
    cursor_.x = 0;
    ++cursor_.y;
    if (cursor_.y >= lines_) {
        for (size_t y = 0; y < (lines_ - 1); ++y)
            memcpy(matrix[y], matrix[y+1], columns_ * sizeof(Char));
        --cursor_.y;
        for (size_t x = 0; x < columns_; ++x)
            matrix[cursor_.y][x] = (Char) {' ', foreground_color_ };
    }
     */
}

void TextLayer::text_advance_cursor()
{
    ++cursor_.x;
    if (cursor_.x >= columns_)
        text_advance_line();
}
