#ifndef TEXT_HH_
#define TEXT_HH_

#include <cstdint>

#include <algorithm>
#include <chrono>
#include <memory>
#include <string>
#include <vector>

#include "layer.hh"
#include "fortuna-protocol.h"

using TimePoint = std::chrono::high_resolution_clock::time_point;
using Time = std::chrono::high_resolution_clock;

struct CursorAttrib {
    uint8_t color;
    bool    visible;
};

struct CharAttrib {
    uint8_t color;
    bool    reverse;
    bool    visible;
};

struct Cursor {
    size_t       x = 0;
    size_t       y = 0;
    CursorAttrib attrib = { COLOR_ORANGE, true };
    bool         blink_state = true;
    TimePoint    last_blink = Time::now();
};

struct Char {
    uint8_t    c      = 0;
    CharAttrib attrib = { COLOR_WHITE, false, false };
};

class Text : public Layer {
public:
    Text() : Layer() {}

    void          set_graphical_mode(GraphicalMode graphical_mode);

    void          set(size_t line, size_t column, Char c);
    void          move_cursor_to(size_t line, size_t column);

    uint8_t       bg_color = COLOR_BLACK;

    void                        update_blink();

    [[nodiscard]] Char const&   get(size_t line, size_t column) const;

    [[nodiscard]] size_t        columns() const { return columns_; }
    [[nodiscard]] size_t        lines() const   { return lines_; }

    [[nodiscard]] Cursor const& cursor() const  { return cursor_; }

    static constexpr size_t Columns_80Columns = 80;
    static constexpr size_t Lines_80Columns   = 30;
    static constexpr size_t Columns_40Columns = 40;
    static constexpr size_t Lines_40Columns   = 25;

private:
    std::unique_ptr<Char[]> matrix_;
    Cursor                  cursor_;

    size_t                  columns_ = 0;
    size_t                  lines_ = 0;

    void  reset_blink();
};

#endif
