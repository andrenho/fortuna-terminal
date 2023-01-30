#include "textlayer.hh"

#include <memory>

TextLayer::TextLayer()
{
    matrix_ = std::make_unique<Char[]>(columns_ * lines_);
    for (size_t i = 0; i < (columns_ * lines_); ++i)
        matrix_[i] = { (uint8_t) (i % 256), Color::WHITE };
}
