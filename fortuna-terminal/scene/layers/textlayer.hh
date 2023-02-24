#ifndef TEXTLAYER_HH_
#define TEXTLAYER_HH_

#include <cstdint>

#include <algorithm>
#include <chrono>
#include <memory>
#include <string>
#include <vector>

#include "layer.hh"
#include "common/geometry.hh"
#include "protocol/mode.hh"

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

struct Cell {
    Char   chr;
    size_t line;
    size_t column;

    Cell(Char const &chr_, size_t line_, size_t column_) : chr(chr_), line(line_), column(column_) {}
};

class TextLayer : public Layer {
public:
    explicit TextLayer(Mode mode);

    void          set(size_t line, size_t column, Char c);
    void          set(std::vector<Cell> const& cells);
    void          move_cursor_to(size_t line, size_t column);

    void          set_mode(Mode mode);

    uint8_t       bg_color = COLOR_BLACK;

    void                        update_blink();

    [[nodiscard]] Char const&   get(size_t line, size_t column) const;

    [[nodiscard]] size_t        columns() const { return columns_; }
    [[nodiscard]] size_t        lines() const   { return lines_; }

    [[nodiscard]] Mode mode() const { return mode_; }

    [[nodiscard]] Cursor const& cursor() const  { return cursor_; }

    static constexpr size_t Columns_80Columns = 80;
    static constexpr size_t Lines_80Columns   = 30;
    static constexpr size_t Columns_40Columns = 42;
    static constexpr size_t Lines_40Columns   = 28;

    void reset();

private:
    std::unique_ptr<Char[]> matrix_ {};
    Cursor                  cursor_ {};

    size_t                  columns_ = 0;
    size_t                  lines_ = 0;

    Mode                    mode_;

    void  reset_blink();
};

#endif
