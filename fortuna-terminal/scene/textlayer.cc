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
    switch (c) {
        case '\n':
            text_advance_line();
            break;
        case '\b':
            if (cursor_.x > 0) {
                --cursor_.x;
                add_char(' ');
                --cursor_.x;
            }
            break;
        default:
            chr(cursor_.y, cursor_.x) = { c, foreground_color_ };
            text_advance_cursor();
    }
    cursor_.blink_state = true;
    cursor_last_blink_ = std::chrono::high_resolution_clock::now();
}

void TextLayer::text_advance_line()
{
    cursor_.x = 0;
    ++cursor_.y;
    if (cursor_.y >= lines_) {
        for (size_t y = 0; y < (lines_ - 1); ++y) {
            for (size_t x = 0; x < columns_; ++x)
                chr(y, x) = chr(y+1, x);
        }
        --cursor_.y;
        for (size_t x = 0; x < columns_; ++x)
            chr(cursor_.y, x) = {' ', foreground_color_ };
    }
}

void TextLayer::text_advance_cursor()
{
    ++cursor_.x;
    if (cursor_.x >= columns_)
        text_advance_line();
}

void TextLayer::move_cursor_relative(int y, int x)
{
    cursor_.x = std::max(0, std::min((int) columns_, cursor_.x + x));
    cursor_.y = std::max(0, std::min((int) lines_, cursor_.y + y));
}

void TextLayer::move_cursor_to(unsigned int x, unsigned int y)
{
    cursor_.x = std::max(0U, std::min(columns_, x - 1));
    cursor_.y = std::max(0U, std::min(lines_, y - 1));
}

void TextLayer::set_color(uint8_t color)
{
    foreground_color_ = (color % Palette::SIZE);
}

void TextLayer::clear_screen()
{
    for (size_t i = 0; i < (columns_ * lines_); ++i)
        matrix_[i] = { ' ', foreground_color_ };
}

void TextLayer::reset_formatting()
{
    foreground_color_ = Color::WHITE;
}
