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

void TextLayer::update()
{
    if (std::chrono::high_resolution_clock::now() > cursor_last_blink_ + 600ms) {
        cursor_.blink_state = !cursor_.blink_state;
        cursor_last_blink_ = std::chrono::high_resolution_clock::now();
    }
}
