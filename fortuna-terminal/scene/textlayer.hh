#ifndef TEXTLAYER_HH_
#define TEXTLAYER_HH_

#include <cstdint>
#include <chrono>
#include <memory>

#include "palette.hh"
#include "layer.hh"

struct Cursor {
    uint8_t  x = 0;
    uint8_t  y = 0;
    uint8_t  color = Color::ORANGE;
    bool     visible = true;
    bool     blink_state = true;
};

struct Char {
    uint8_t chr;
    uint8_t color;
};

class TextLayer : public Layer {
public:
    TextLayer();

    void update_blink();

    [[nodiscard]] Cursor const& cursor() const { return cursor_; }
    [[nodiscard]] Char const&   chr(uint16_t line, uint16_t column) const { return matrix_[line * columns_ + column]; }
    [[nodiscard]] unsigned int  columns() const { return columns_; }
    [[nodiscard]] unsigned int  lines() const { return lines_; }

    [[nodiscard]] uint8_t background_color() const { return background_color_; }

    void add_char(uint8_t c);

    void move_cursor_relative(int y, int x);
    void move_cursor_to(unsigned int x, unsigned int y);
    void set_color(uint8_t color);
    void clear_screen();
    void reset_formatting();
    void clear_to_beginning_of_line();
    void clear_to_end_of_line();
    void clear_line();

    void set_scroll_region(uint8_t start, uint8_t end);

    void delete_char_under_cursor();

private:
    unsigned int            columns_ = 40;
    unsigned int            lines_ = 25;
    unsigned int            scroll_start_ = 1;
    unsigned int            scroll_end_ = lines_;

    uint8_t background_color_ = Color::BLACK;
    uint8_t foreground_color_ = Color::WHITE;

    Cursor cursor_;
    std::unique_ptr<Char[]> matrix_ = nullptr;
    std::chrono::time_point<std::chrono::system_clock> cursor_last_blink_;

    Char& chr(uint16_t line, uint16_t column) { return matrix_[line * columns_ + column]; }

    void text_advance_line();
    void text_advance_cursor();
    void tab();
};

#endif //TEXTLAYER_HH_
