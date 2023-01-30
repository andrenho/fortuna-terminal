#ifndef TEXTLAYER_HH_
#define TEXTLAYER_HH_

#include <cstdint>
#include <memory>

#include "palette.hh"
#include "layer.hh"

struct Cursor {
    uint8_t  x = 0;
    uint8_t  y = 0;
    uint8_t  color = Color::ORANGE;
    bool     visible = true;
    bool     blink_state = true;
    uint64_t last_blink = 0;
};

struct Char {
    uint8_t chr;
    uint8_t color;
};

class TextLayer : public Layer {
public:
    TextLayer();

    uint8_t                 background_color = Color::BLACK;

    Cursor const& cursor() const { return cursor_; }
    Char const& chr(uint16_t line, uint16_t column) const { return matrix_[line * columns_ + column]; }

    unsigned int columns() const { return columns_; }
    unsigned int lines() const { return lines_; }

private:
    unsigned int            columns_ = 40;
    unsigned int            lines_ = 25;

    Cursor                  cursor_;
    std::unique_ptr<Char[]> matrix_ = nullptr;
};

#endif //TEXTLAYER_HH_
